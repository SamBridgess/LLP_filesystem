#include "nfs.h"
#include "stuff.h"

#include <time.h>

#include <sys/sysmacros.h>

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


mountres3 *
mountproc3_mnt_3_svc(dirpath *argp, struct svc_req *rqstp)
{
    printf("mountproc3_mnt_3_svc\n");

    static mountres3  result;
    static int auth = AUTH_UNIX;

    size_t path_len = strlen(*argp);
    char file_handle[32] = {0};
    char *dpath = *argp;


    printf("generated filehandle: %zu ", path_len);
    for(int i = 0; i < 32; i++) {
        if(i < path_len)
            file_handle[i] = *(dpath + i);
        else
            file_handle[i] = '#';
        printf("%c", file_handle[i]);
    }
    printf("\n");


    result.fhs_status = MNT3_OK;
    result.mountres3_u.mountinfo.fhandle.fhandle3_len = 32;
    result.mountres3_u.mountinfo.fhandle.fhandle3_val = file_handle;
    result.mountres3_u.mountinfo.auth_flavors.auth_flavors_len = 1;
    result.mountres3_u.mountinfo.auth_flavors.auth_flavors_val = &auth;

    return &result;
}

// Function to convert struct timespec to nfstime3
nfstime3 timespec_to_nfstime3(const struct timespec *ts) {
    nfstime3 result;
    result.seconds = ts->tv_sec;
    result.nseconds = ts->tv_nsec;
    return result;
}

FSINFO3res *
nfsproc3_fsinfo_3_svc(FSINFO3args *argp, struct svc_req *rqstp)
{
    printf("nfsproc3_fsinfo_3_svc\n");


    static FSINFO3res result;
    char *path;
    unsigned int maxdata;
    printf("%s\n", argp->fsroot.data.data_val);

    if (get_socket_type(rqstp) == SOCK_STREAM)
        maxdata = NFS_MAXDATA_TCP;
    else
        maxdata = NFS_MAXDATA_UDP;

    struct stat st;
    stat("/home/sam/nfs-give", &st);
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.type = 2;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.mode = st.st_mode & 0777;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.nlink = st.st_nlink;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.uid = st.st_uid;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.gid = st.st_size;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.size = st.st_size;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.used = st.st_size;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.rdev.specdata1 = major(st.st_rdev);
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.rdev.specdata2 = minor(st.st_rdev);
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.fsid = st.st_dev;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.fileid = st.st_ino;
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.atime = timespec_to_nfstime3(&st.st_atim);
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.mtime = timespec_to_nfstime3(&st.st_mtim);
    result.FSINFO3res_u.resok.obj_attributes.post_op_attr_u.attributes.ctime = timespec_to_nfstime3(&st.st_ctim);



    result.FSINFO3res_u.resok.obj_attributes.attributes_follow = 1;


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
    char *path;

    struct stat st;
    stat("/home/sam/nfs-give", &st);
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.type = 2;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.mode = st.st_mode & 0777;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.nlink = st.st_nlink;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.uid = st.st_uid;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.gid = st.st_size;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.size = st.st_size;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.used = st.st_size;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.rdev.specdata1 = major(st.st_rdev);
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.rdev.specdata2 = minor(st.st_rdev);
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.fsid = st.st_dev;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.fileid = st.st_ino;
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.atime = timespec_to_nfstime3(&st.st_atim);
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.mtime = timespec_to_nfstime3(&st.st_mtim);
    result.PATHCONF3res_u.resok.obj_attributes.post_op_attr_u.attributes.ctime = timespec_to_nfstime3(&st.st_ctim);


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
    char *path;
    post_op_attr post;

    struct stat st;
    stat("/home/sam/nfs-give", &st);
    result.GETATTR3res_u.resok.obj_attributes.type = 2;
    result.GETATTR3res_u.resok.obj_attributes.mode = st.st_mode & 0777;
    result.GETATTR3res_u.resok.obj_attributes.nlink = st.st_nlink;
    result.GETATTR3res_u.resok.obj_attributes.uid = st.st_uid;
    result.GETATTR3res_u.resok.obj_attributes.gid = st.st_size;
    result.GETATTR3res_u.resok.obj_attributes.size = st.st_size;
    result.GETATTR3res_u.resok.obj_attributes.used = st.st_size;
    result.GETATTR3res_u.resok.obj_attributes.rdev.specdata1 = major(st.st_rdev);
    result.GETATTR3res_u.resok.obj_attributes.rdev.specdata2 = minor(st.st_rdev);
    result.GETATTR3res_u.resok.obj_attributes.fsid = st.st_dev;
    result.GETATTR3res_u.resok.obj_attributes.fileid = st.st_ino;
    result.GETATTR3res_u.resok.obj_attributes.atime = timespec_to_nfstime3(&st.st_atim);
    result.GETATTR3res_u.resok.obj_attributes.mtime = timespec_to_nfstime3(&st.st_mtim);
    result.GETATTR3res_u.resok.obj_attributes.ctime = timespec_to_nfstime3(&st.st_ctim);


    result.status = NFS3_OK;

    return &result;
}