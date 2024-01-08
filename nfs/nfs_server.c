#include "nfs.h"
#include "stuff.h"
#include <time.h>
#include <sys/sysmacros.h>
#include <dirent.h>

void generate_file_handle(char *file_handle, size_t length) {
    if (length < 16) {
        fprintf(stderr, "Error: Insufficient buffer length for file handle\n");
        exit(EXIT_FAILURE);
    }
    time_t current_time = time(NULL);
    if (current_time == -1) {
        perror("Error getting current time");
        exit(EXIT_FAILURE);
    }
    pid_t pid = getpid();
    uint64_t combined_value = ((uint64_t)current_time << 32) | (uint32_t)pid;
    memcpy(file_handle, &combined_value, sizeof(uint64_t));
}
nfstime3 timespec_to_nfstime3(const struct timespec *ts) {
    nfstime3 result;
    result.seconds = ts->tv_sec;
    result.nseconds = ts->tv_nsec;
    return result;
}
void fill_attributes(char *path, fattr3 *attributes) {
    struct stat st;
    stat(path, &st);

    attributes->type = 2;
    attributes->mode = st.st_mode & 0777;
    attributes->nlink = st.st_nlink;
    attributes->uid = st.st_uid;
    attributes->gid = st.st_size;
    attributes->size = st.st_size;
    attributes->used = st.st_size;
    attributes->rdev.specdata1 = major(st.st_rdev);
    attributes->rdev.specdata2 = minor(st.st_rdev);
    attributes->fsid = st.st_dev;
    attributes->fileid = st.st_ino;
    attributes->atime = timespec_to_nfstime3(&st.st_atim);
    attributes->mtime = timespec_to_nfstime3(&st.st_mtim);
    attributes->ctime = timespec_to_nfstime3(&st.st_ctim);
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
            // This is the first entry
            firstEntry = newEntry;
        }

        prevEntry = newEntry;
    }

    closedir(dir);

    *entries = firstEntry;
    *eof = 0;
}



mountres3 *
mountproc3_mnt_3_svc(dirpath *argp, struct svc_req *rqstp)
{
    static mountres3  result;
    static int auth = AUTH_UNIX;
    char *path = *argp;

    printf("generated filehandle: %s\n", path);

    result.fhs_status = MNT3_OK;
    result.mountres3_u.mountinfo.fhandle.fhandle3_len = strlen(path);
    result.mountres3_u.mountinfo.fhandle.fhandle3_val = path;
    result.mountres3_u.mountinfo.auth_flavors.auth_flavors_len = 1;
    result.mountres3_u.mountinfo.auth_flavors.auth_flavors_val = &auth;

    return &result;
}

FSINFO3res *
nfsproc3_fsinfo_3_svc(FSINFO3args *argp, struct svc_req *rqstp)
{
    static FSINFO3res result;
    unsigned int maxdata;
    char *path = argp->fsroot.data.data_val;
    printf("Received smth: %s\n", path);

    if (get_socket_type(rqstp) == SOCK_STREAM)
        maxdata = NFS_MAXDATA_TCP;
    else
        maxdata = NFS_MAXDATA_UDP;


    result.FSINFO3res_u.resok.obj_attributes.attributes_follow = 1;
    fill_attributes(path, &result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes);

    result.status = NFS3_OK;
    result.FSINFO3res_u.resok.rtmax = maxdata;
    result.FSINFO3res_u.resok.rtpref = maxdata;
    result.FSINFO3res_u.resok.rtmult = 4096;
    result.FSINFO3res_u.resok.wtmax = maxdata;
    result.FSINFO3res_u.resok.wtpref = maxdata;
    result.FSINFO3res_u.resok.wtmult = 4096;
    result.FSINFO3res_u.resok.dtpref = 4096;
    result.FSINFO3res_u.resok.maxfilesize = ~0ULL;
    result.FSINFO3res_u.resok.time_delta.seconds = 1;
    result.FSINFO3res_u.resok.time_delta.nseconds = 0;
    result.FSINFO3res_u.resok.properties = backend_fsinfo_properties;

    return &result;
}

PATHCONF3res *
nfsproc3_pathconf_3_svc(PATHCONF3args *argp, struct svc_req *rqstp)
{
    static PATHCONF3res result;
    char *path = argp->object.data.data_val;


    result.PATHCONF3res_u.resok.obj_attributes.attributes_follow = 1;
    fill_attributes(path, &result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes);

    result.status = NFS3_OK;
    result.PATHCONF3res_u.resok.linkmax = 0xFFFFFFFF;
    result.PATHCONF3res_u.resok.name_max = NFS_MAXPATHLEN;
    result.PATHCONF3res_u.resok.no_trunc = TRUE;
    result.PATHCONF3res_u.resok.chown_restricted = FALSE;
    result.PATHCONF3res_u.resok.case_insensitive = FALSE;
    result.PATHCONF3res_u.resok.case_preserving = TRUE;

    return &result;
}

GETATTR3res *
nfsproc3_getattr_3_svc(GETATTR3args *argp, struct svc_req *rqstp)
{
    static GETATTR3res result;
    char *path = argp->object.data.data_val;

    fill_attributes(path, &result.GETATTR3res_u.resok.obj_attributes);

    result.status = NFS3_OK;

    return &result;
}

ACCESS3res *
nfsproc3_access_3_svc(ACCESS3args *argp, struct svc_req *rqstp)
{
    static ACCESS3res result;
    char *path = argp->object.data.data_val;

    result.ACCESS3res_u.resok.obj_attributes.attributes_follow = 1;
    fill_attributes(path, &result.ACCESS3res_u.resok.obj_attributes.post_op_attr_u.attributes);

    result.status = NFS3_OK;
    result.ACCESS3res_u.resok.access = ACCESS3_READ | ACCESS3_LOOKUP | ACCESS3_MODIFY | ACCESS3_EXTEND | ACCESS3_DELETE | ACCESS3_EXECUTE;

    return &result;
}

READDIR3res *
nfsproc3_readdir_3_svc(READDIR3args *argp, struct svc_req *rqstp)
{
    static READDIR3res  result;
    char *path = argp->dir.data.data_val;


    entry3 *entries = NULL;
    int eof;

    fill_entries_for_readdir(path, &entries, &eof);

    if (eof) {
        result.READDIR3res_u.resok.reply.eof = 1;  // TRUE
    } else {
        result.READDIR3res_u.resok.reply.eof = 0;  // FALSE
    }

    result.READDIR3res_u.resok.reply.entries = entries;
    result.READDIR3res_u.resok.dir_attributes.attributes_follow = 1;  // TRUE

    fill_attributes(path, &result.READDIR3res_u.resok.dir_attributes.post_op_attr_u.attributes);

    return &result;
}