#include "nfs.h"
#include "stuff.h"
#include "read.h"
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <dirent.h>

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
    printf("MNT\n");

    static mountres3  result;
    static int auth = AUTH_UNIX;
    char *path = *argp;

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
    printf("FSINFO\n");

    static FSINFO3res result;
    unsigned int maxdata;
    char *path = argp->fsroot.data.data_val;

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
    printf("PATHCONF\n");

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
    printf("GETATTR\n");
    static GETATTR3res result;
    char *path = argp->object.data.data_val;
    //path[strlen(path) - 1] = '\0';


    fill_attributes(path, &result.GETATTR3res_u.resok.obj_attributes);

    result.status = NFS3_OK;

    return &result;
}

ACCESS3res *
nfsproc3_access_3_svc(ACCESS3args *argp, struct svc_req *rqstp)
{

    static ACCESS3res result;
    char *path = argp->object.data.data_val;
    int len = argp->object.data.data_len;
    path[len] = '\0';

    printf("ACCESS    %d %s\n",len, path);

    result.ACCESS3res_u.resok.obj_attributes.attributes_follow = 1;
    fill_attributes(path, &result.ACCESS3res_u.resok.obj_attributes.post_op_attr_u.attributes);

    result.status = NFS3_OK;
    result.ACCESS3res_u.resok.access = ACCESS3_READ | ACCESS3_LOOKUP | ACCESS3_MODIFY | ACCESS3_EXTEND | ACCESS3_DELETE | ACCESS3_EXECUTE;

    return &result;
}

READDIR3res *
nfsproc3_readdir_3_svc(READDIR3args *argp, struct svc_req *rqstp)
{
    printf("READDIR\n");
    static READDIR3res  result;
    char *path = argp->dir.data.data_val;
    int len = argp->dir.data.data_len;
    path[len] = '\0';

    entry3 *entries = NULL;
    int eof;

    fill_entries_for_readdir(path, &entries, &eof);

    if (eof) {
        result.READDIR3res_u.resok.reply.eof = 0;  // TRUE
    } else {
        result.READDIR3res_u.resok.reply.eof = 1;  // FALSE
    }

    result.READDIR3res_u.resok.reply.entries = entries;
    result.READDIR3res_u.resok.dir_attributes.attributes_follow = 1;  // TRUE

    fill_attributes(path, &result.READDIR3res_u.resok.dir_attributes.post_op_attr_u.attributes);

    return &result;
}

LOOKUP3res *
nfsproc3_lookup_3_svc(LOOKUP3args *argp, struct svc_req *rqstp)
{
    static LOOKUP3res  result;
    char *dirPath = argp->what.dir.data.data_val;
    int dirPathLen = argp->what.dir.data.data_len;
    dirPath[dirPathLen] = '\0';
    char *fileName = argp->what.name;

    char *path = (char *) malloc(strlen(dirPath) + strlen(fileName) + 1);
    strcpy(path, dirPath);
    if(dirPath[strlen(dirPath) - 1] != '/')
        strcat(path, "/");
    strcat(path, fileName);


    result.LOOKUP3res_u.resok.dir_attributes.attributes_follow = 1;
    fill_attributes(dirPath, &result.LOOKUP3res_u.resok.dir_attributes.post_op_attr_u.attributes);

    printf("LOOKUP     %s (%s + %s)\n", path, dirPath, fileName);

    if(fill_attributes(path, &result.LOOKUP3res_u.resok.obj_attributes.post_op_attr_u.attributes) == 0){
        printf("sending LOOOKUP NFS3_OK\n");
        result.LOOKUP3res_u.resok.obj_attributes.attributes_follow = 1;
        result.status = NFS3_OK;
        result.LOOKUP3res_u.resok.object.data.data_val = path;
        result.LOOKUP3res_u.resok.object.data.data_len = strlen(path);
    } else {
        printf("sending LOOOKUP NFS3ERR_NOENT\n");
        result.LOOKUP3res_u.resok.obj_attributes.attributes_follow = 0;
        result.status = NFS3ERR_NOENT;
    }

    return &result;
}

READ3res *
nfsproc3_read_3_svc(READ3args *argp, struct svc_req *rqstp)
{
    printf("READ\n");

    static READ3res  result;
    char *path = argp->file.data.data_val;
    int len = argp->file.data.data_len;
    path[len] = '\0';

    off_t offset = argp->offset;
    size_t count = 1024;

    fill_READ3res_stream(path, offset, count, &result);

    //   result.status = NFS3_OK;

    return &result;
}

WRITE3res *
nfsproc3_write_3_svc(WRITE3args *argp, struct svc_req *rqstp)
{
    static WRITE3res  result;

    return &result;
}

CREATE3res *
nfsproc3_create_3_svc(CREATE3args *argp, struct svc_req *rqstp)
{
    static CREATE3res  result;

    /*
     * insert server code here
     */

    return &result;
}