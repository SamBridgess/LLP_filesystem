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
    unsigned int  root_cluster_start;
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
};

unsigned int DIR_ENTRY_SIZE = sizeof(struct fat_dir_entry);

char *fat_table = NULL;
char *fat_disk_path = NULL;
FILE *fat_disk = NULL;

unsigned int min(unsigned int a, unsigned int b) {
    return (a < b) ? a : b;
}