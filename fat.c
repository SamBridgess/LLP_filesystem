#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#define _GNU_SOURCE

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <assert.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#define FAT_DISK_SIZE 10485760 // 10 MB = 10485760 B
#define FAT_DISK_NAME "fat_disk"

#define DIR_ENTRY_MAX_NAME_LEN 21
#define DIR_ENTRY_TYPE_FILE 0
#define DIR_ENTRY_TYPE_SUBDIR 1
#define DIR_ENTRY_TYPE_SYMLINK 2

#define BYTE_PER_CLUSTER 4096
#define CLUSTER_COUNT 2560
#define CLUSTER_PER_FAT 3
#define FAT_TYPE 32
#define ROOT_CLUSTER_START 4
#define FREE_CLUSTER_START 5

#define END_OF_CHAIN 0

struct fat_superblock {
    unsigned int byte_per_cluster;
    unsigned int cluster_count;
    unsigned int cluster_per_fat;
    unsigned int fat_type;
    unsigned int root_cluster_start;
    unsigned int free_cluster_start;
};

union {
    struct fat_superblock s;
    char pad[512];

} superblock;

struct fat_dir_entry {
    char name[DIR_ENTRY_MAX_NAME_LEN + 1];
    unsigned short int type;
    unsigned int size;
    unsigned int first_cluster;
    time_t time;
};

unsigned int DIR_ENTRY_SIZE = sizeof(struct fat_dir_entry);

char *fat_table = NULL;
char *fat_disk_path = NULL;
FILE *fat_disk = NULL;

unsigned int min(unsigned int a, unsigned int b) {
    return (a < b) ? a : b;
}

/**
 * Get parent path from given path
 * Example: /a/b/c -> /a/b/
 */
char *get_parent_path(const char* path) {
    unsigned int name_start = (unsigned int) strlen(path);
    for (; path[name_start - 1] != '/'; name_start--);
    char *parent_path = (char *) malloc(name_start);
    strncpy(parent_path, path, name_start);
    parent_path[name_start - 1] = '\0'; // Null terminate
    return parent_path;
}

/**
 * Get target name from given path
 * Example: /a/b/c -> c
 */
char *get_target_name(const char* path) {
    unsigned int name_len = (unsigned int) strlen(path);
    unsigned int name_start = name_len;
    for (; path[name_start - 1] != '/'; name_start--);
    name_len -= name_start;
    char *target_name = (char *) malloc(name_len + 1);
    strncpy(target_name, path + name_start, name_len);
    target_name[name_len] = '\0'; // Null terminate
    return target_name;
}

/**
 * Get disk offset from cluster number
 */
unsigned int get_disk_offset(unsigned int cluster_num) {
    assert(cluster_num < superblock.s.cluster_count);
    return cluster_num * superblock.s.byte_per_cluster;
}

/**
 * Return a pointer to a new cluster in memory.
 * Must be freed if no longer in use.
 */
char *get_new_cluster() {
    char *cluster = (char *) calloc(1, superblock.s.byte_per_cluster);
    assert(cluster != NULL);
    return cluster;
}

/**
 * Read a cluster from disk and return a pointer to the cluster
 * If the pointer is no longer used, it must be deallocated with free()
 * On failure, NULL is returned
 */
int read_cluster_from_disk(char *buf, unsigned int cluster_num, unsigned int offset, unsigned int size) {
    unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
    assert(cluster_num < superblock.s.cluster_count &&
           offset + size <= byte_per_cluster);
    unsigned int byte_read = (unsigned int) pread(fileno(fat_disk), buf, size, get_disk_offset(cluster_num) + offset);
    return (byte_read < size) ? -1 : 0;
}

int read_entire_cluster_from_disk(char *buf, unsigned int cluster_num) {
    return read_cluster_from_disk(buf, cluster_num, 0, superblock.s.byte_per_cluster);
}

/**
 * Write data to the physical cluster on disk with the given cluster_num
 * Write at most the number of bytes per cluster
 * Return 0 on success, -1 on failure
 */
int write_cluster_to_disk(const char *buf, unsigned int cluster_num) {
    assert(cluster_num < superblock.s.cluster_count);
    unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
    return (pwrite(fileno(fat_disk), buf, byte_per_cluster,
                   get_disk_offset(cluster_num)) < byte_per_cluster) ? -1 : 0;
}

/**
 * Write superblock to disk
 * Return 0 on success, -1 on failure
 */
int write_superblock_to_disk() {
    char *tmp_cluster = get_new_cluster();
    memcpy(tmp_cluster, &superblock, sizeof(superblock));
    int result = write_cluster_to_disk(tmp_cluster, 0);
    free(tmp_cluster);
    return result;
}

/**
 * Return the value in the FAT table (in-memory) for the given cluster number
 */
unsigned int read_fat_entry(unsigned int cluster_num) {
    unsigned int byte_to_copy = superblock.s.fat_type / 8;
    unsigned int offset = (cluster_num - 1 - superblock.s.cluster_per_fat) * byte_to_copy;
    unsigned int result = 0;
    memcpy(&result, fat_table + offset, byte_to_copy);
    return result;
}

unsigned int get_last_cluster_in_chain(unsigned int start_cluster) {
    if (start_cluster == END_OF_CHAIN) {
        return END_OF_CHAIN;
    }
    unsigned int last_cluster;
    unsigned int tmp_cluster = start_cluster;
    do {
        last_cluster = tmp_cluster;
    } while((tmp_cluster = read_fat_entry(tmp_cluster)) != END_OF_CHAIN);
    return last_cluster;
}

/**
 * Return the number of clusters in a chain starting at the given cluster number
 */
unsigned int count_cluster_in_chain(unsigned int cluster_num) {
    assert(cluster_num >= superblock.s.root_cluster_start || cluster_num == 0);
    return (cluster_num == END_OF_CHAIN) ? 0 : 1 + count_cluster_in_chain(read_fat_entry(cluster_num));
}

/**
 * Update the value in the FAT table for the given cluster number with the given value
 * If write_to_disk is true, then the changes will also be written to the physical disk
 * Return 0 on success, -1 on failure
 */
int update_fat_entry(unsigned int cluster_num, unsigned int value, bool write_to_disk) {
    assert(cluster_num >= 1 + superblock.s.cluster_per_fat &&
           cluster_num < superblock.s.cluster_count);
    unsigned int byte_to_copy = superblock.s.fat_type / 8;
    unsigned int offset = (cluster_num - 1 - superblock.s.cluster_per_fat) * byte_to_copy;
    memcpy(fat_table + offset, &value, byte_to_copy);

    if (write_to_disk) {
        unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
        unsigned int fat_cluster_num = offset / byte_per_cluster;
        return write_cluster_to_disk(fat_table + fat_cluster_num * byte_per_cluster,
                                     fat_cluster_num + 1);
    }

    return 0;
}

/**
 * Check if there is an available free cluster
 */
bool has_free_cluster() {
    return superblock.s.free_cluster_start > superblock.s.root_cluster_start;
}

/**
 * Allocate a free cluster from the front of the free list, update the free list,
 * and set the newly allocated free cluster to point to 0 (EOC) in the FAT table.
 * The allocated cluster is zeroed out to prevent data leak.
 * Return the number of the allocated free cluster
 */
unsigned int pop_free_clusters(unsigned int length) {
    assert(length > 0);
    unsigned int first_cluster = superblock.s.free_cluster_start;

    // Find last cluster in the freed chain
    unsigned int last_cluster = first_cluster;
    while (--length > 0 && last_cluster != END_OF_CHAIN) {
        last_cluster = read_fat_entry(last_cluster);
    }
    if (length > 0) {
        return END_OF_CHAIN;
    }

    // Make the cluster after the last cluster in the freed chain become the head of free list
    superblock.s.free_cluster_start = read_fat_entry(last_cluster);
    write_superblock_to_disk();

    // Make the last cluster the end of chain
    update_fat_entry(last_cluster, END_OF_CHAIN, true);

    // Zero out allocated clusters
    char *zeros = (char *) calloc(1, superblock.s.byte_per_cluster);
    unsigned int cur_cluster = first_cluster;
    do {
        write_cluster_to_disk(zeros, cur_cluster);
    } while ((cur_cluster = read_fat_entry(cur_cluster)) != END_OF_CHAIN);
    free(zeros);

    return first_cluster;
}

/**
 * Add a free cluster to the end of the chain starting from cluster_num
 * Return the number of the allocated free cluster
 */
unsigned int add_free_cluster(unsigned int cluster_num) {
    unsigned int free_cluster = pop_free_clusters(1);
    unsigned int next_cluster;
    while ((next_cluster = read_fat_entry(cluster_num)) != END_OF_CHAIN) {
        cluster_num = next_cluster;
    }
    update_fat_entry(cluster_num, free_cluster, true);
    return free_cluster;
}

/**
 * Add a chain of cluster starting from the given cluster_num back to the free list
 * The start of the chain will be the new head of the free list, and the end of chain
 * will point to the previous head of the free list.
 * Always succeed, even if the chain is already in the free list, which could cause an error.
 */
void free_cluster_chain(unsigned int start_cluster_num) {
    unsigned int end_cluster_num = start_cluster_num;
    unsigned int next_cluster;
    while ((next_cluster = read_fat_entry(end_cluster_num)) != END_OF_CHAIN) {
        end_cluster_num = next_cluster;
    }
    update_fat_entry(end_cluster_num, superblock.s.free_cluster_start, true);
    superblock.s.free_cluster_start = start_cluster_num;
    write_superblock_to_disk();
}

/**
 * Read directory entries in the given cluster at the given offset
 * Fill the results into buf with filler
 */
void read_dir_entries(unsigned int cluster_num, unsigned int offset,
                      void *buf, fuse_fill_dir_t filler) {
    unsigned int tmp_offset = offset;
    while (tmp_offset >= superblock.s.byte_per_cluster) {
        cluster_num = read_fat_entry(cluster_num);
        tmp_offset -= superblock.s.byte_per_cluster;
    }

    char *cluster = get_new_cluster();
    while (cluster_num != 0) {
        assert(read_entire_cluster_from_disk(cluster, cluster_num) != -1);
        while (tmp_offset < superblock.s.byte_per_cluster) {
            struct fat_dir_entry dir_entry;
            memcpy(&dir_entry, cluster + tmp_offset, DIR_ENTRY_SIZE);
            tmp_offset += DIR_ENTRY_SIZE;
            offset += DIR_ENTRY_SIZE;
            if (dir_entry.name[0] == '\0') {
                continue;
            }
            if (filler(buf, dir_entry.name, NULL, offset) != 0) {
                free(cluster);
                return;
            }
        }
        cluster_num = read_fat_entry(cluster_num);
        tmp_offset = 0;
    }

    free(cluster);
}

/**
 * Create a directory entry for subdirectory in the given cluster.
 * The entry will have the given name and the first_cluster_num as first cluster.
 * The location of the entry will be the first free entry found.
 * If there is no free entry in the current cluster, the method will search in the
 * next cluster as read from FAT.
 * Return 0 on success, -1 on failure
 */
int add_dir_entry_to_cluster(unsigned int cluster_num, const char *dir_name,
                             unsigned int first_cluster_num, unsigned short int type) {
    assert(first_cluster_num >= 1 + superblock.s.cluster_per_fat &&
           first_cluster_num < superblock.s.cluster_count);
    size_t dir_name_len = strlen(dir_name);
    if (dir_name_len > DIR_ENTRY_MAX_NAME_LEN || dir_name_len == 0) {
        return -1;
    }

    unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
    unsigned int offset;
    char *cluster = get_new_cluster();
    assert(read_entire_cluster_from_disk(cluster, cluster_num) != -1);
    bool found_free_entry = false;

    for (offset = 0; offset < byte_per_cluster; offset += DIR_ENTRY_SIZE) {
        struct fat_dir_entry dir_entry;
        memcpy(&dir_entry, cluster + offset, DIR_ENTRY_SIZE);
        if (dir_entry.name[0] == '\0') {
            found_free_entry = true;
            break;
        }
    }

    if (!found_free_entry) {
        free(cluster);
        unsigned int next_cluster = read_fat_entry(cluster_num);
        if (next_cluster != 0) {
            return add_dir_entry_to_cluster(next_cluster, dir_name, first_cluster_num, type);
        } else if (!has_free_cluster()) {
            return -1;
        }

        offset = 0;
        cluster_num = add_free_cluster(cluster_num);
        cluster = get_new_cluster();
    }

    // Create dir entry
    struct fat_dir_entry dir_entry;
    dir_entry.type = type;
    dir_entry.size = 0;
    dir_entry.first_cluster = first_cluster_num;
    dir_entry.time = time(NULL);
    strncpy(dir_entry.name, dir_name, DIR_ENTRY_MAX_NAME_LEN + 1);

    // Write to disk
    memcpy(cluster + offset, &dir_entry, DIR_ENTRY_SIZE);
    int result = write_cluster_to_disk(cluster, cluster_num);
    free(cluster);
    return result;
}

/**
 * Check if directory is empty
 */
bool is_dir_empty(unsigned int cluster_num) {
    assert(cluster_num >= superblock.s.root_cluster_start && cluster_num != 0);
    char *cluster = get_new_cluster();
    assert(read_entire_cluster_from_disk(cluster, cluster_num) != -1);

    for (unsigned int offset = 0; offset < superblock.s.byte_per_cluster; offset += DIR_ENTRY_SIZE) {
        struct fat_dir_entry dir_entry;
        memcpy(&dir_entry, cluster + offset, DIR_ENTRY_SIZE);
        if ((strcmp(dir_entry.name, ".") != 0) && (strcmp(dir_entry.name, "..") != 0) &&
            dir_entry.name[0] != '\0') {
            free(cluster);
            return false;
        }
    }

    free(cluster);
    unsigned int next_cluster = read_fat_entry(cluster_num);
    return (next_cluster == 0) ? true : is_dir_empty(next_cluster);
}

/**
 * Finds the directory entry with the give name in the given cluster and fills the info in the given dir_entry
 * If the entry is not found in the current cluster and the cluster is not end-of-chain,
 * then the function will search in the next cluster in the chain.
 * Returns 0 on success, -1 on failure.
 */
int find_dir_entry_by_name(unsigned int cluster_num, const char *name,
                           struct fat_dir_entry *dir_entry) {
    char *cluster = get_new_cluster();
    assert(read_entire_cluster_from_disk(cluster, cluster_num) != -1);

    for (unsigned int offset = 0; offset < superblock.s.byte_per_cluster; offset += DIR_ENTRY_SIZE) {
        memcpy(dir_entry, cluster + offset, DIR_ENTRY_SIZE);
        if (strcmp(dir_entry->name, name) == 0) {
            free(cluster);
            return 0;
        }
    }

    free(cluster);
    unsigned int next_cluster = read_fat_entry(cluster_num);
    return (next_cluster == 0) ? -1 : find_dir_entry_by_name(next_cluster, name, dir_entry);
}

/**
 * Finds the directory entry for the given path and fill the info in the given dir_entry
 * Returns 0 on success, -1 on failure.
 */
int find_dir_entry_by_path(const char *path, struct fat_dir_entry *dir_entry) {
    if (strcmp(path, "/") <= 0) {
        return -1;
    }

    unsigned int cluster_num = superblock.s.root_cluster_start;
    char *orig, *remainder, *tok;

    char *parent_path = get_parent_path(path);
    char *target_name = get_target_name(path);
    orig = remainder = strdup(parent_path);
    strsep(&remainder, "/"); // Skip first token because it's empty

    while ((tok = strsep(&remainder, "/")) != NULL) {
        if (find_dir_entry_by_name(cluster_num, tok, dir_entry) == -1 ||
            dir_entry->type != DIR_ENTRY_TYPE_SUBDIR) {
            free(orig);
            return -1;
        }
        cluster_num = dir_entry->first_cluster;
    }

    free(orig);
    free(parent_path);

    if (find_dir_entry_by_name(cluster_num, target_name, dir_entry) == -1) {
        free(target_name);
        return -1;
    }

    free(target_name);
    return 0;
}

/**
 * Find the first cluster for the file/directory at the given path
 */
unsigned int find_first_cluster_by_path(const char *path) {
    unsigned int cluster_num = superblock.s.root_cluster_start;
    if (strcmp(path, "/") > 0) { // If not root
        struct fat_dir_entry dir_entry;
        assert(find_dir_entry_by_path(path, &dir_entry) != -1);
        cluster_num = dir_entry.first_cluster;
    }
    return cluster_num;
}

/**
 * Add a directory entry as specified in the path with the given type
 * Exampe: /a/b/c -> Create dir/file 'c' in /a/b/
 */
int add_dir_entry(const char *path, unsigned short int type) {
    // Extract the name of the directory to create and the parent path
    char *parent_path = get_parent_path(path);
    char *target_name = get_target_name(path);

    // Find cluster of parent
    unsigned int parent_cluster = superblock.s.root_cluster_start;
    if (strcmp(parent_path, "/") > 0) { // If parent is not root
        struct fat_dir_entry dir_entry;
//        dir_entry.time = time(NULL);
        assert(find_dir_entry_by_path(parent_path, &dir_entry) != -1);
        parent_cluster = dir_entry.first_cluster;
    }
    free(parent_path);

    // Add the directory entry
    unsigned int first_cluster = pop_free_clusters(1);
    add_dir_entry_to_cluster(parent_cluster, target_name, first_cluster, type);
    if (type == DIR_ENTRY_TYPE_SUBDIR) { // Add 2 children "." and ".."
        add_dir_entry_to_cluster(first_cluster, ".", first_cluster, type);
        add_dir_entry_to_cluster(first_cluster, "..", parent_cluster, type);
    }

    free(target_name);
    return 0;
}

/**
 * Update the directory entry with the same name as the given dir_entry
 * in the cluster chain starting with the given cluster num
 * Returns 0 on success, -1 on failure.
 */
int update_dir_entry(unsigned int cluster_num, struct fat_dir_entry *dir_entry, bool remove) {
    char *cluster = get_new_cluster();
    assert(read_entire_cluster_from_disk(cluster, cluster_num) != -1);

    for (unsigned int offset = 0; offset < superblock.s.byte_per_cluster; offset += DIR_ENTRY_SIZE) {
        struct fat_dir_entry tmp_dir_entry;
        memcpy(&tmp_dir_entry, cluster + offset, DIR_ENTRY_SIZE);
        if (strcmp(tmp_dir_entry.name, dir_entry->name) == 0) {
            if (remove) {
                bzero(cluster + offset, 1); // Zero out the first character of the dir entry's name
            } else {
                memcpy(cluster + offset, dir_entry, DIR_ENTRY_SIZE);
            }
            int result = write_cluster_to_disk(cluster, cluster_num);
            free(cluster);
            return result;
        }
    }

    free(cluster);
    unsigned int next_cluster = read_fat_entry(cluster_num);
    return (next_cluster == END_OF_CHAIN) ? -1 : update_dir_entry(next_cluster, dir_entry, remove);
}

/**
 * Write to a file, minimizing the number of read as much as possible.
 * If file size is changed, update the directory entry of the file in the parent cluster
 */
int update_file(unsigned int parent_cluster, struct fat_dir_entry dir_entry,
                const char *buf, size_t size, off_t offset) {

    assert(dir_entry.type == DIR_ENTRY_TYPE_FILE);

    // Check if enough clusters to write to.
    // If not, allocate more from free list and update the cluster chain and FAT
    // If there is not enough cluster, return ENOMEM
    unsigned int first_file_cluster = dir_entry.first_cluster;
    unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
    unsigned int num_cluster_allocated = count_cluster_in_chain(first_file_cluster);
    unsigned int final_byte_to_write = ((unsigned int) offset) + (unsigned int) size - 1;
    unsigned int final_cluster_to_write = final_byte_to_write / byte_per_cluster + 1;

    if (final_cluster_to_write > num_cluster_allocated) { // If not enough cluster to write to
        unsigned int num_cluster_to_allocate = final_cluster_to_write - num_cluster_allocated;
        unsigned int first_freed_cluster = pop_free_clusters(num_cluster_to_allocate);
        if (first_freed_cluster == END_OF_CHAIN) {
            return -ENOMEM;
        }

        unsigned int last_cluster = get_last_cluster_in_chain(first_file_cluster);
        update_fat_entry(last_cluster, first_freed_cluster, true);
    }

    if (final_byte_to_write + 1 > dir_entry.size) { // Update dir entry size
        dir_entry.size = final_byte_to_write + 1;
        dir_entry.time = time(NULL);
        assert(update_dir_entry(parent_cluster, &dir_entry, false) != -1);
    }

    // Find first cluster to write
    unsigned int cur_cluster = first_file_cluster;
    for (unsigned int i = (unsigned int) offset / byte_per_cluster; i > 0; i--) {
        cur_cluster = read_fat_entry(cur_cluster);
    }

    // Write to first cluster. If first cluster is written to fully, write w/o reading
    unsigned int remaining_bytes = (unsigned int) size;
    unsigned int within_cluster_offset = (unsigned int) offset % byte_per_cluster;
    unsigned int num_byte_to_write = min(remaining_bytes, byte_per_cluster - within_cluster_offset);
    char *cluster = get_new_cluster();

    if (num_byte_to_write < byte_per_cluster) {
        assert(read_entire_cluster_from_disk(cluster, cur_cluster) != -1);
        memcpy(cluster + within_cluster_offset, buf, num_byte_to_write);
        assert(write_cluster_to_disk(cluster, cur_cluster) != -1);
    } else {
        assert(write_cluster_to_disk(buf, cur_cluster) != -1);
    }

    remaining_bytes -= num_byte_to_write;
    if (remaining_bytes == 0) {
        free(cluster);
        return (int) size;
    }

    // Write to remaining clusters. Always write w/o reading for all clusters before last
    // If last cluster is written to completely, also write w/o reading
    unsigned int write_last_cluster = (remaining_bytes % byte_per_cluster == 0) ? 0 : 1;
    unsigned int num_remaining_clusters = remaining_bytes / byte_per_cluster + write_last_cluster;
    unsigned int buf_offset = num_byte_to_write;
    while (num_remaining_clusters > write_last_cluster) {
        cur_cluster = read_fat_entry(cur_cluster);
        assert(write_cluster_to_disk(buf + buf_offset, cur_cluster) != -1);
        buf_offset += byte_per_cluster;
        num_remaining_clusters--;
        remaining_bytes -= byte_per_cluster;
    }

    // If the final few remaining bytes don't take up an entire cluster, read, modify, write
    if (write_last_cluster == 1) {
        cur_cluster = read_fat_entry(cur_cluster);
        assert(read_entire_cluster_from_disk(cluster, cur_cluster) != -1);
        memcpy(cluster, buf + buf_offset, remaining_bytes);
        assert(write_cluster_to_disk(cluster, cur_cluster) != -1);
    }

    free(cluster);
    return (int) size;
}

/**
 * Initialize superblock
 * Return 0 on success, -1 on failure
 */
int init_superblock(bool first_time) {
    assert(fat_disk != NULL);
    if (!first_time) {
        ssize_t result = pread(fileno(fat_disk), &superblock, sizeof(superblock), 0);
        return (result < 0 || (size_t) result < sizeof(superblock)) ? -1 : 0;
    }
    superblock.s = (struct fat_superblock) {
            BYTE_PER_CLUSTER, CLUSTER_COUNT, CLUSTER_PER_FAT,
            FAT_TYPE, ROOT_CLUSTER_START, FREE_CLUSTER_START
    };
    return write_superblock_to_disk();
}

/**
 * Initialize FAT table
 * Return 0 on success, -1 on failure
 */
int init_fat(bool first_time) {
    assert(fat_disk != NULL);
    unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
    unsigned int fat_size = byte_per_cluster * superblock.s.cluster_per_fat;
    fat_table = (char *) calloc(1, fat_size);

    if (!first_time) {
        return (pread(fileno(fat_disk), fat_table, fat_size,
                      byte_per_cluster) < fat_size) ? -1 : 0;
    }

    update_fat_entry(superblock.s.root_cluster_start, 0, false);
    update_fat_entry(superblock.s.cluster_count - 1, 0, false);
    for (unsigned int i = superblock.s.free_cluster_start;
         i < superblock.s.cluster_count - 1; i++) {
        update_fat_entry(i, i + 1, false);
    }

    return (pwrite(fileno(fat_disk), fat_table, fat_size,
                   byte_per_cluster) < fat_size) ? -1 : 0;
}

static void* fat_init(struct fuse_conn_info *conn) {
    (void) conn;
    if (access(fat_disk_path, F_OK) == -1) { // If disk file doesn't exist
        fat_disk = fopen(fat_disk_path, "w+");
        // Create disk file with 10 MB of 0's
        assert(ftruncate(fileno(fat_disk), FAT_DISK_SIZE) != -1);

        // Create and write superblock and FAT to disk
        assert(init_superblock(true) != -1);
        assert(init_fat(true) != -1);

        // Add "." and ".." directory entries for root
        assert(add_dir_entry_to_cluster(superblock.s.root_cluster_start, ".",
                                        superblock.s.root_cluster_start,
                                        DIR_ENTRY_TYPE_SUBDIR) != -1);
        assert(add_dir_entry_to_cluster(superblock.s.root_cluster_start, "..",
                                        superblock.s.root_cluster_start,
                                        DIR_ENTRY_TYPE_SUBDIR) != -1);

        fflush(fat_disk);
        fat_disk = freopen(fat_disk_path, "r+", fat_disk);

    } else {
        fat_disk = fopen(fat_disk_path, "r+");
        // Read superblock and FAT from disk to memory
        assert(init_superblock(false) != -1);
        assert(init_fat(false) != -1);

    }

    // Make update to file immediately flushed (aka unbuffered)
    assert(setvbuf(fat_disk, NULL, _IONBF, 0) == 0);
    return NULL;
}

static void fat_destroy(void *private_data) {
    (void) private_data;
    free(fat_disk_path);
    free(fat_table);
    fclose(fat_disk);
    return;
}

static int fat_getattr(const char *path, struct stat *stbuf) {
    unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
    if (strcmp(path, "/") == 0) {
        bzero(stbuf, sizeof(struct stat));
        stbuf->st_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
        stbuf->st_nlink = 2;
        stbuf->st_ino = superblock.s.root_cluster_start;
        stbuf->st_size = byte_per_cluster * count_cluster_in_chain(superblock.s.root_cluster_start);
        return 0;
    }

    // Get directory entry for the file
    struct fat_dir_entry dir_entry;
    if (find_dir_entry_by_path(path, &dir_entry) == -1) {
        return -ENOENT;
    }

    // Set stat
    bzero(stbuf, sizeof(struct stat));
    stbuf->st_nlink = 2;
    stbuf->st_ino = dir_entry.first_cluster;
    stbuf->st_mode = S_IRWXU | S_IRWXG | S_IRWXO;


    struct timespec my_time;
    my_time.tv_sec = dir_entry.time;
    stbuf->st_mtim = my_time;

    if (dir_entry.type == DIR_ENTRY_TYPE_SUBDIR) {
        stbuf->st_mode = stbuf->st_mode | S_IFDIR;
        stbuf->st_size = byte_per_cluster * count_cluster_in_chain(dir_entry.first_cluster);
    } else if (dir_entry.type == DIR_ENTRY_TYPE_FILE) {
        stbuf->st_mode = stbuf->st_mode | S_IFREG;
        stbuf->st_size = dir_entry.size;
    } else {
        stbuf->st_mode = stbuf->st_mode | S_IFLNK;
    }

    return 0;
}

static int fat_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    return fat_getattr(path, stbuf);
}

static int fat_access(const char *path, int mask) {
    (void) mask;
    if (strcmp(path, "/") == 0) {
        return 0;
    }
    struct fat_dir_entry dir_entry;
    return (find_dir_entry_by_path(path, &dir_entry) == -1) ? -ENOENT : 0;
}

static int fat_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    unsigned int cluster_num = find_first_cluster_by_path(path);
    read_dir_entries(cluster_num, (unsigned int) offset, buf, filler);
    return 0;
}

static int fat_mkdir(const char *path, mode_t mode) {
    (void) mode;
    if (!has_free_cluster()) {
        return -ENOMEM;
    }
    return add_dir_entry(path, DIR_ENTRY_TYPE_SUBDIR);
}

static int fat_rmdir(const char *path) {
    char *parent_path = get_parent_path(path);
    char *target_name = get_target_name(path);

    // Find cluster of parent
    unsigned int parent_cluster = find_first_cluster_by_path(parent_path);
    free(parent_path);

    // Find the directory entry for the directory to remove
    struct fat_dir_entry dir_entry;
    assert(find_dir_entry_by_name(parent_cluster, target_name, &dir_entry) != -1);
    free(target_name);

    if (!is_dir_empty(dir_entry.first_cluster)) { // Check if empty
        return -ENOTEMPTY;
    }
    if (dir_entry.type != DIR_ENTRY_TYPE_SUBDIR) { // Check if not directory
        return -ENOTDIR;
    }

    free_cluster_chain(dir_entry.first_cluster);
    return update_dir_entry(parent_cluster, &dir_entry, true);
}

static int fat_statfs(const char *path, struct statvfs *stbuf) {
    (void) path;
    bzero(stbuf, sizeof(struct statvfs));
    unsigned int free_block_count = count_cluster_in_chain(superblock.s.free_cluster_start);
    stbuf->f_blocks = superblock.s.cluster_count - superblock.s.cluster_per_fat - 1;
    stbuf->f_bsize = superblock.s.byte_per_cluster;
    stbuf->f_bfree = free_block_count;
    stbuf->f_bavail = free_block_count;
    stbuf->f_ffree = free_block_count;
    stbuf->f_favail = free_block_count;
    stbuf->f_namemax = DIR_ENTRY_MAX_NAME_LEN;
    return 0;
}

static int fat_mknod(const char *path, mode_t mode, dev_t rdev) {
    (void) rdev;
    if (S_ISREG(mode) == 0) {
        return -EACCES;
    }
    if (!has_free_cluster()) {
        return -ENOMEM;
    }
    return add_dir_entry(path, DIR_ENTRY_TYPE_FILE);
}

static int fat_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    (void) fi;
    fat_mknod(path, mode | S_IFREG, 0);
    return 0;
}

static int fat_open(const char *path, struct fuse_file_info *fi) {
    (void) fi;
    struct fat_dir_entry dir_entry;
    return (find_dir_entry_by_path(path, &dir_entry) == -1) ? -ENOENT : 0;
}

static int fat_read(const char *path, char *buf,
                    size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    assert(strcmp(path, "/") > 0);
    struct fat_dir_entry dir_entry;
    assert(find_dir_entry_by_path(path, &dir_entry) != -1);

    if (offset >= dir_entry.size || size == 0) { // If offset is at or past end of file or size is 0
        return 0;
    }

    // Modify size in order to not read past EOF
    size = (size_t) min((unsigned int) dir_entry.size - (unsigned int) offset, (unsigned int) size);

    // Find first cluster to read
    unsigned int byte_per_cluster = superblock.s.byte_per_cluster;
    unsigned int cur_cluster = dir_entry.first_cluster;
    for (unsigned int i = (unsigned int) offset / byte_per_cluster; i > 0; i--) {
        cur_cluster = read_fat_entry(cur_cluster);
    }

    // Read all clusters
    unsigned int within_cluster_offset = (unsigned int) offset % byte_per_cluster;
    unsigned int remaining_bytes = (unsigned int) size;
    unsigned int buf_offset = 0;
    char *cluster = get_new_cluster();

    while (remaining_bytes > 0) {
        unsigned int num_byte_to_read = min(remaining_bytes, byte_per_cluster - within_cluster_offset);
        assert(read_cluster_from_disk(buf + buf_offset, cur_cluster, 0, num_byte_to_read) != -1);
        cur_cluster = read_fat_entry(cur_cluster); // Get next cluster to read
        remaining_bytes -= num_byte_to_read;
        buf_offset += num_byte_to_read;
        within_cluster_offset = 0;
    }

    free(cluster);
    return (int) size;
}

static int fat_write(const char *path, const char *buf,
                     size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    char *parent_path = get_parent_path(path);
    char *target_name = get_target_name(path);

    // Find cluster of parent
    unsigned int parent_cluster = find_first_cluster_by_path(parent_path);
    free(parent_path);

    // Find the directory entry for the file to edit
    struct fat_dir_entry dir_entry;
    assert(find_dir_entry_by_name(parent_cluster, target_name, &dir_entry) != -1);
    free(target_name);

    return update_file(parent_cluster, dir_entry, buf, size, offset);
}

static int fat_truncate(const char *path, off_t size) {
    char *parent_path = get_parent_path(path);
    char *target_name = get_target_name(path);

    // Find cluster of parent
    unsigned int parent_cluster = find_first_cluster_by_path(parent_path);
    free(parent_path);

    // Find the directory entry for the file to edit
    struct fat_dir_entry dir_entry;
    assert(find_dir_entry_by_name(parent_cluster, target_name, &dir_entry) != -1);
    free(target_name);

    if (size == dir_entry.size) {
        return 0;
    }

    if (size > dir_entry.size) { // Extend
        unsigned int num_zeros = (unsigned int) size - dir_entry.size;
        char *zeros = (char *) calloc(1, num_zeros);
        int result = update_file(parent_cluster, dir_entry, zeros, num_zeros, dir_entry.size);
        free(zeros);
        return result == (int) num_zeros ? 0 : -1;
    }

    // Shrink
    unsigned int new_last_cluster = dir_entry.first_cluster;
    for (unsigned int i = ((unsigned int) size / superblock.s.byte_per_cluster); i > 0; i--) {
        new_last_cluster = read_fat_entry(new_last_cluster);
    }
    unsigned int after_new_last_cluster = read_fat_entry(new_last_cluster);
    update_fat_entry(new_last_cluster, END_OF_CHAIN, true);

    // Put remaining clusters back into free list
    if (after_new_last_cluster != END_OF_CHAIN) {
        unsigned int previous_last_cluster = after_new_last_cluster;
        unsigned int tmp_cluster;
        while ((tmp_cluster = read_fat_entry(previous_last_cluster)) != END_OF_CHAIN) {
            previous_last_cluster = tmp_cluster;
        }
        update_fat_entry(previous_last_cluster, superblock.s.free_cluster_start, true);
        superblock.s.free_cluster_start = after_new_last_cluster;
        assert(write_superblock_to_disk() != -1);
    }

    // Update size
    dir_entry.size = (unsigned int) size;
    assert(update_dir_entry(parent_cluster, &dir_entry, false) != -1);

    return 0;
}

static int fat_release(const char *path, struct fuse_file_info *fi) {
    (void) path;
    (void) fi;
    return 0;
}



static int fat_unlink(const char *path) {
    // Extract the parent path and the name of the file to remove
    char *parent_path = get_parent_path(path);
    char *target_name = get_target_name(path);

    // Find cluster of parent
    unsigned int parent_cluster = find_first_cluster_by_path(parent_path);
    free(parent_path);

    // Find the directory entry of the file to remove
    struct fat_dir_entry dir_entry;
    assert(find_dir_entry_by_name(parent_cluster, target_name, &dir_entry) != -1);
    free(target_name);

    if (dir_entry.type != DIR_ENTRY_TYPE_FILE) { // Check if not directory
        return -1;
    }

    free_cluster_chain(dir_entry.first_cluster); // Free all clusters of the file
    return update_dir_entry(parent_cluster, &dir_entry, true); // Remove dir entry from parent dir
}

static int fat_utimens(const char* name, const struct timespec tv[2]){
    (void) name;
    (void) tv[2];
    return 0;
}

static struct fuse_operations fat_operations = {
        .init			= fat_init,
        .destroy		= fat_destroy,
        .getattr		= fat_getattr,
        .fgetattr		= fat_fgetattr,
        .access		= fat_access,
        .readdir		= fat_readdir,
        .mkdir		= fat_mkdir,
        .rmdir		= fat_rmdir,
        .statfs		= fat_statfs,
        .create		= fat_create,
        .mknod		= fat_mknod,
        .open			= fat_open,
        .read			= fat_read,
        .write		= fat_write,
        .truncate		= fat_truncate,
        .release		= fat_release,
        .unlink		= fat_unlink,
        .utimens = fat_utimens,
};

int main(int argc, char *argv[]) {
    umask(0);
    char *cur_working_dir = get_current_dir_name();
    fat_disk_path = (char *) malloc(strlen(cur_working_dir) + strlen(FAT_DISK_NAME) + 2);
    if (asprintf(&fat_disk_path, "%s%s%s", cur_working_dir, "/", FAT_DISK_NAME) == -1) {
        exit(1);
    };
    free(cur_working_dir);
    return fuse_main(argc, argv, &fat_operations, NULL);
}