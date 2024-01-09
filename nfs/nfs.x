const NFS3_FHSIZE = 64;
const NFS3_COOKIEVERFSIZE = 8;
const NFS3_CREATEVERFSIZE = 8;
const NFS3_WRITEVERFSIZE = 8;


typedef unsigned hyper uint64;
typedef hyper int64;
typedef unsigned long uint32;
typedef long int32;
typedef string filename3<>;
typedef string nfspath3<>;
typedef uint64 fileid3;
typedef uint64 cookie3;
typedef opaque cookieverf3[NFS3_COOKIEVERFSIZE];

typedef opaque createverf3[NFS3_CREATEVERFSIZE];
typedef opaque writeverf3[NFS3_WRITEVERFSIZE];
typedef uint32 uid3;
typedef uint64 size3;
typedef uint32 gid3;
typedef uint64 offset3;
typedef uint32 mode3;
typedef uint32 count3;


const MNTPATHLEN = 1024;  /* Maximum bytes in a path name */
const MNTNAMLEN  = 255;   /* Maximum bytes in a name */
const FHSIZE3    = 64;    /* Maximum bytes in a V3 file handle */

/* MOUNT -----------------------------------------------------*/
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


struct nfs_fh3 {
    opaque data<NFS3_FHSIZE>;
};

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


/* FSINFO -----------------------------------------------------*/

enum nfsstat3 {
    NFS3_OK             = 0,
    NFS3ERR_PERM        = 1,
    NFS3ERR_NOENT       = 2,
    NFS3ERR_IO          = 5,
    NFS3ERR_NXIO        = 6,
    NFS3ERR_ACCES       = 13,
    NFS3ERR_EXIST       = 17,
    NFS3ERR_XDEV        = 18,
    NFS3ERR_NODEV       = 19,
    NFS3ERR_NOTDIR      = 20,
    NFS3ERR_ISDIR       = 21,
    NFS3ERR_INVAL       = 22,
    NFS3ERR_FBIG        = 27,
    NFS3ERR_NOSPC       = 28,
    NFS3ERR_ROFS        = 30,
    NFS3ERR_MLINK       = 31,
    NFS3ERR_NAMETOOLONG = 63,
    NFS3ERR_NOTEMPTY    = 66,
    NFS3ERR_DQUOT       = 69,
    NFS3ERR_STALE       = 70,
    NFS3ERR_REMOTE      = 71,
    NFS3ERR_BADHANDLE   = 10001,
    NFS3ERR_NOT_SYNC    = 10002,
    NFS3ERR_BAD_COOKIE  = 10003,
    NFS3ERR_NOTSUPP     = 10004,
    NFS3ERR_TOOSMALL    = 10005,
    NFS3ERR_SERVERFAULT = 10006,
    NFS3ERR_BADTYPE     = 10007,
    NFS3ERR_JUKEBOX     = 10008
};

const FSF3_LINK        = 0x0001;
const FSF3_SYMLINK     = 0x0002;
const FSF3_HOMOGENEOUS = 0x0008;
const FSF3_CANSETTIME  = 0x0010;

struct FSINFO3args {
    nfs_fh3   fsroot;
};
struct nfstime3 {
    uint32   seconds;
    uint32   nseconds;
};
struct specdata3 {
    uint32     specdata1;
    uint32     specdata2;
};
enum ftype3 {
    NF3REG    = 1,
    NF3DIR    = 2,
    NF3BLK    = 3,
    NF3CHR    = 4,
    NF3LNK    = 5,
    NF3SOCK   = 6,
    NF3FIFO   = 7
};
struct fattr3 {
    ftype3     type;
    mode3      mode;
    uint32     nlink;
    uid3       uid;
    gid3       gid;
    size3      size;
    size3      used;
    specdata3  rdev;
    uint64     fsid;
    fileid3    fileid;
    nfstime3   atime;
    nfstime3   mtime;
    nfstime3   ctime;
};

union post_op_attr switch (bool attributes_follow) {
    case TRUE:
        fattr3   attributes;
    case FALSE:
        void;
};



struct FSINFO3resok {
    post_op_attr obj_attributes;
    uint32       rtmax;
    uint32       rtpref;
    uint32       rtmult;
    uint32       wtmax;
    uint32       wtpref;
    uint32       wtmult;
    uint32       dtpref;

    size3        maxfilesize;
    nfstime3     time_delta;
    uint32       properties;
};

struct FSINFO3resfail {
    post_op_attr obj_attributes;
};

union FSINFO3res switch (nfsstat3 status) {
    case NFS3_OK:
        FSINFO3resok   resok;
    default:
        FSINFO3resfail resfail;
};


/* PATHCONF-------------------------------------------------------------*/
struct PATHCONF3args {
    nfs_fh3   object;
};

struct PATHCONF3resok {
    post_op_attr obj_attributes;
    uint32       linkmax;
    uint32       name_max;
    bool         no_trunc;
    bool         chown_restricted;
    bool         case_insensitive;
    bool         case_preserving;
};

struct PATHCONF3resfail {
    post_op_attr obj_attributes;
};

union PATHCONF3res switch (nfsstat3 status) {
    case NFS3_OK:
        PATHCONF3resok   resok;
    default:
        PATHCONF3resfail resfail;
};
/* GETATTR -----------------------------------------------------------------*/
struct GETATTR3args {
    nfs_fh3  object;
};

struct GETATTR3resok {
    fattr3   obj_attributes;
};

union GETATTR3res switch (nfsstat3 status) {
    case NFS3_OK:
        GETATTR3resok  resok;
    default:
        void;
};
/* ACCESS --------------------------------------------------------------*/
const ACCESS3_READ    = 0x0001;
const ACCESS3_LOOKUP  = 0x0002;
const ACCESS3_MODIFY  = 0x0004;
const ACCESS3_EXTEND  = 0x0008;
const ACCESS3_DELETE  = 0x0010;
const ACCESS3_EXECUTE = 0x0020;

struct ACCESS3args {
    nfs_fh3  object;
    uint32   access;
};

struct ACCESS3resok {
    post_op_attr   obj_attributes;
    uint32         access;
};

struct ACCESS3resfail {
    post_op_attr   obj_attributes;
};
union ACCESS3res switch (nfsstat3 status) {
    case NFS3_OK:
        ACCESS3resok   resok;
    default:
        ACCESS3resfail resfail;
};
/* READDIR ---------------------------------------------------------------*/

struct READDIR3args {
    nfs_fh3      dir;
    cookie3      cookie;
    cookieverf3  cookieverf;
    count3       count;
};
struct entry3 {
    fileid3      fileid;
    filename3    name;
    cookie3      cookie;
    entry3       *nextentry;
};

struct dirlist3 {
    entry3       *entries;
    bool         eof;
};

struct READDIR3resok {
    post_op_attr dir_attributes;
    cookieverf3  cookieverf;
    dirlist3     reply;
};

struct READDIR3resfail {
    post_op_attr dir_attributes;
};
union READDIR3res switch (nfsstat3 status) {
    case NFS3_OK:
        READDIR3resok   resok;
    default:
        READDIR3resfail resfail;
};



/* LOOKUP ---------------------------------------------------------------*/
struct diropargs3 {
    nfs_fh3     dir;
    filename3   name;
};
struct LOOKUP3args {
    diropargs3  what;
};

struct LOOKUP3resok {
    nfs_fh3      object;
    post_op_attr obj_attributes;
    post_op_attr dir_attributes;
};

struct LOOKUP3resfail {
    post_op_attr dir_attributes;
};

union LOOKUP3res switch (nfsstat3 status) {
    case NFS3_OK:
        LOOKUP3resok    resok;
    default:
        LOOKUP3resfail  resfail;
};
/* READ ----------------------------------------------------------*/
struct READ3args {
    nfs_fh3  file;
    offset3  offset;
    count3   count;
};

struct READ3resok {
    post_op_attr   file_attributes;
    count3         count;
    bool           eof;
    opaque         data<>;
};

struct READ3resfail {
    post_op_attr   file_attributes;
};

union READ3res switch (nfsstat3 status) {
    case NFS3_OK:
        READ3resok   resok;
    default:
        READ3resfail resfail;
};
/* ---------------------------------------------------------------*/


program NFS_PROGRAM {
    version NFS_V3 {
        /*
        void NFSPROC3_NULL(void) = 0;
        */
        GETATTR3res NFSPROC3_GETATTR(GETATTR3args) = 1;
        /*
        SETATTR3res NFSPROC3_SETATTR(SETATTR3args) = 2;
        */
        LOOKUP3res NFSPROC3_LOOKUP(LOOKUP3args)  = 3;
        ACCESS3res NFSPROC3_ACCESS(ACCESS3args) = 4;
        /*
        READLINK3res NFSPROC3_READLINK(READLINK3args) = 5;
        */
        READ3res NFSPROC3_READ(READ3args) = 6;
        /*
        WRITE3res NFSPROC3_WRITE(WRITE3args) = 7;
        CREATE3res NFSPROC3_CREATE(CREATE3args) = 8;
        MKDIR3res NFSPROC3_MKDIR(MKDIR3args) = 9;
        SYMLINK3res NFSPROC3_SYMLINK(SYMLINK3args) = 10;
        MKNOD3res NFSPROC3_MKNOD(MKNOD3args) = 11;
        REMOVE3res NFSPROC3_REMOVE(REMOVE3args) = 12;
        RMDIR3res NFSPROC3_RMDIR(RMDIR3args) = 13;
        RENAME3res NFSPROC3_RENAME(RENAME3args) = 14;
        LINK3res NFSPROC3_LINK(LINK3args) = 15;
        */
        READDIR3res NFSPROC3_READDIR(READDIR3args) = 16;
        /*
        READDIRPLUS3res NFSPROC3_READDIRPLUS(READDIRPLUS3args) = 17;
        FSSTAT3res NFSPROC3_FSSTAT(FSSTAT3args) = 18;
        */
        FSINFO3res NFSPROC3_FSINFO(FSINFO3args) = 19;
        PATHCONF3res NFSPROC3_PATHCONF(PATHCONF3args) = 20;
        /*
        COMMIT3res NFSPROC3_COMMIT(COMMIT3args) = 21;
        */
    } = 3;
} = 100003;