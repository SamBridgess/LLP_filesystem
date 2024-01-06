const MNTPATHLEN = 1024;  /* Maximum bytes in a path name */
const MNTNAMLEN  = 255;   /* Maximum bytes in a name */
const FHSIZE3    = 64;    /* Maximum bytes in a V3 file handle */

enum mountstat3 {
    MNT3_OK = 0,                 /* no error */
    MNT3ERR_PERM = 1,            /* Not owner */
    MNT3ERR_NOENT = 2,           /* No such file or directory */
    MNT3ERR_IO = 5,              /* I/O error */
    MNT3ERR_ACCES = 13,          /* Permission denied */
    MNT3ERR_NOTDIR = 20,         /* Not a directory */
    MNT3ERR_INVAL = 22,          /* Invalid argument */
    MNT3ERR_NAMETOOLONG = 63,    /* Filename too long */
    MNT3ERR_NOTSUPP = 10004,     /* Operation not supported */
    MNT3ERR_SERVERFAULT = 10006  /* A failure on the server */
};
typedef opaque fhandle3<FHSIZE3>;
typedef string dirpath<MNTPATHLEN>;
typedef string name<MNTNAMLEN>;

struct mountres3_ok {
    fhandle3   fhandle;
    int        auth_flavors<>;
};

union mountres3 switch (mountstat3 fhs_status) {
    case MNT3_OK:
        mountres3_ok  mountinfo;
    default:
        void;
};

program MOUNT {
    version MOUNT_VERSION {
        mountres3 MOUNTPROC3_MNT(dirpath) = 1;
    } = 3;
} = 100005;



program NFS {
    version NFS_VERSION {
        string READ(string) = 1;
    } = 3;
} = 100003;