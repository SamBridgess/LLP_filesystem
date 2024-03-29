#include "nfs.h"
#include "tools.h"
#include "read.h"


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
    int len = argp->object.data.data_len;
    path[len] = '\0';


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
        result.READDIR3res_u.resok.reply.eof = 0;
    } else {
        result.READDIR3res_u.resok.reply.eof = 1;
    }

    result.READDIR3res_u.resok.reply.entries = entries;
    result.READDIR3res_u.resok.dir_attributes.attributes_follow = 1;

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


    char *path = (char *) malloc(strlen(dirPath) + strlen(fileName) + 2);
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
        result.LOOKUP3res_u.resfail.dir_attributes.attributes_follow = 0;
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
    size_t count = argp->count;

    fill_READ3res_stream(path, offset, count, &result);

    return &result;
}