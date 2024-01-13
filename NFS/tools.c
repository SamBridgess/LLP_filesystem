#include "tools.h"


nfstime3 timespec_to_nfstime3(const struct timespec *ts) {
    nfstime3 result;
    result.seconds = ts->tv_sec;
    result.nseconds = ts->tv_nsec;
    return result;
}
int fill_attributes(char *path, fattr3 *attributes) {
    printf("looking for path: %s\n", path);

    struct stat st;
    if(stat(path, &st) == -1) {
        printf("error finding: %s\n", path);
        return -1;
    }

    int type = -1;
    if(S_ISREG(st.st_mode))
        type = 1;
    if(S_ISDIR(st.st_mode))
        type = 2;

    attributes->type = type;
    attributes->mode = st.st_mode;
    attributes->nlink = st.st_nlink;
    attributes->uid = st.st_uid;
    attributes->gid = st.st_gid;
    attributes->size = st.st_size;
    attributes->used = st.st_size;
    attributes->rdev.specdata1 = major(st.st_rdev);
    attributes->rdev.specdata2 = minor(st.st_rdev);
    attributes->fsid = st.st_dev;
    attributes->fileid = st.st_ino;
    attributes->atime = timespec_to_nfstime3(&st.st_atim);
    attributes->mtime = timespec_to_nfstime3(&st.st_mtim);
    attributes->ctime = timespec_to_nfstime3(&st.st_ctim);
    return 0;
}
void fill_entries_for_readdir(const char *dirPath, entry3 **entries, int *eof) {
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        perror("opendir");
        *eof = 1;
        return;
    }

    struct dirent *dirEntry;
    entry3 *prevEntry = NULL;
    entry3 *firstEntry = NULL;

    while ((dirEntry = readdir(dir)) != NULL) {
        entry3 *newEntry = malloc(sizeof(entry3));
        if (newEntry == NULL) {
            perror("malloc");
            *eof = 1;
            closedir(dir);
            return;
        }

        newEntry->fileid = dirEntry->d_ino;
        newEntry->name = dirEntry->d_name;
        newEntry->cookie = dirEntry->d_off;
        newEntry->nextentry = NULL;

        if (prevEntry != NULL) {
            prevEntry->nextentry = newEntry;
        } else {
            firstEntry = newEntry;
        }

        prevEntry = newEntry;
    }

    closedir(dir);

    *entries = firstEntry;
    *eof = 0;
}

int get_socket_type(struct svc_req *rqstp) {
    int v, res;
    socklen_t l;
    l = sizeof(v);
    res = getsockopt(1, SOL_SOCKET, SO_TYPE, &v, &l);
    if (res < 0) return -1;
    return v;
}


u_int fh_length(const unfs3_fh_t * fh) {
    return fh->len + sizeof(fh->len) + sizeof(fh->dev) + sizeof(fh->ino) +
           sizeof(fh->gen) + sizeof(fh->pwhash);
}

//Convert a unfs3_fh_t to a nfs_fh3
nfs_fh3 fh_encode(const unfs3_fh_t *fh, char *buffer) {
    nfs_fh3 handle;
    char *buf;

    assert(fh);
    assert(buffer);

    handle.data.data_len = fh_length(fh);
    handle.data.data_val = buffer;

    buf = buffer;

    memcpy(buf, &fh->dev, sizeof(fh->dev));
    buf += sizeof(fh->dev);
    memcpy(buf, &fh->ino, sizeof(fh->ino));
    buf += sizeof(fh->ino);
    memcpy(buf, &fh->gen, sizeof(fh->gen));
    buf += sizeof(fh->gen);
    memcpy(buf, &fh->pwhash, sizeof(fh->pwhash));
    buf += sizeof(fh->pwhash);
    memcpy(buf, &fh->len, sizeof(fh->len));
    buf += sizeof(fh->len);
    if (fh->len)
        memcpy(buf, fh->inos, fh->len);

    return handle;
}
