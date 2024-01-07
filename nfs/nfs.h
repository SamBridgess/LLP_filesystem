/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _NFS_H_RPCGEN
#define _NFS_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NFS3_FHSIZE 64
#define NFS3_COOKIEVERFSIZE 8
#define NFS3_CREATEVERFSIZE 8
#define NFS3_WRITEVERFSIZE 8

typedef u_quad_t uint64;

typedef quad_t int64;

typedef u_long uint32;

typedef long int32;

typedef char *filename3;

typedef char *nfspath3;

typedef uint64 fileid3;

typedef uint64 cookie3;

typedef char cookieverf3[NFS3_COOKIEVERFSIZE];

typedef char createverf3[NFS3_CREATEVERFSIZE];

typedef char writeverf3[NFS3_WRITEVERFSIZE];

typedef uint32 uid3;

typedef uint64 size3;

typedef uint32 gid3;

typedef uint64 offset3;

typedef uint32 mode3;

typedef uint32 count3;
#define MNTPATHLEN 1024
#define MNTNAMLEN 255
#define FHSIZE3 64

enum mountstat3 {
	MNT3_OK = 0,
	MNT3ERR_PERM = 1,
	MNT3ERR_NOENT = 2,
	MNT3ERR_IO = 5,
	MNT3ERR_ACCES = 13,
	MNT3ERR_NOTDIR = 20,
	MNT3ERR_INVAL = 22,
	MNT3ERR_NAMETOOLONG = 63,
	MNT3ERR_NOTSUPP = 10004,
	MNT3ERR_SERVERFAULT = 10006,
};
typedef enum mountstat3 mountstat3;

typedef struct {
	u_int fhandle3_len;
	char *fhandle3_val;
} fhandle3;

typedef char *dirpath;

typedef char *name;

struct nfs_fh3 {
	struct {
		u_int data_len;
		char *data_val;
	} data;
};
typedef struct nfs_fh3 nfs_fh3;

struct mountres3_ok {
	fhandle3 fhandle;
	struct {
		u_int auth_flavors_len;
		int *auth_flavors_val;
	} auth_flavors;
};
typedef struct mountres3_ok mountres3_ok;

struct mountres3 {
	mountstat3 fhs_status;
	union {
		mountres3_ok mountinfo;
	} mountres3_u;
};
typedef struct mountres3 mountres3;

enum nfsstat3 {
	NFS3_OK = 0,
	NFS3ERR_PERM = 1,
	NFS3ERR_NOENT = 2,
	NFS3ERR_IO = 5,
	NFS3ERR_NXIO = 6,
	NFS3ERR_ACCES = 13,
	NFS3ERR_EXIST = 17,
	NFS3ERR_XDEV = 18,
	NFS3ERR_NODEV = 19,
	NFS3ERR_NOTDIR = 20,
	NFS3ERR_ISDIR = 21,
	NFS3ERR_INVAL = 22,
	NFS3ERR_FBIG = 27,
	NFS3ERR_NOSPC = 28,
	NFS3ERR_ROFS = 30,
	NFS3ERR_MLINK = 31,
	NFS3ERR_NAMETOOLONG = 63,
	NFS3ERR_NOTEMPTY = 66,
	NFS3ERR_DQUOT = 69,
	NFS3ERR_STALE = 70,
	NFS3ERR_REMOTE = 71,
	NFS3ERR_BADHANDLE = 10001,
	NFS3ERR_NOT_SYNC = 10002,
	NFS3ERR_BAD_COOKIE = 10003,
	NFS3ERR_NOTSUPP = 10004,
	NFS3ERR_TOOSMALL = 10005,
	NFS3ERR_SERVERFAULT = 10006,
	NFS3ERR_BADTYPE = 10007,
	NFS3ERR_JUKEBOX = 10008,
};
typedef enum nfsstat3 nfsstat3;
#define FSF3_LINK 0x0001
#define FSF3_SYMLINK 0x0002
#define FSF3_HOMOGENEOUS 0x0008
#define FSF3_CANSETTIME 0x0010

struct FSINFO3args {
	nfs_fh3 fsroot;
};
typedef struct FSINFO3args FSINFO3args;

struct nfstime3 {
	uint32 seconds;
	uint32 nseconds;
};
typedef struct nfstime3 nfstime3;

struct specdata3 {
	uint32 specdata1;
	uint32 specdata2;
};
typedef struct specdata3 specdata3;

enum ftype3 {
	NF3REG = 1,
	NF3DIR = 2,
	NF3BLK = 3,
	NF3CHR = 4,
	NF3LNK = 5,
	NF3SOCK = 6,
	NF3FIFO = 7,
};
typedef enum ftype3 ftype3;

struct fattr3 {
	ftype3 type;
	mode3 mode;
	uint32 nlink;
	uid3 uid;
	gid3 gid;
	size3 size;
	size3 used;
	specdata3 rdev;
	uint64 fsid;
	fileid3 fileid;
	nfstime3 atime;
	nfstime3 mtime;
	nfstime3 ctime;
};
typedef struct fattr3 fattr3;

struct post_op_attr {
	bool_t attributes_follow;
	union {
		fattr3 attributes;
	} post_op_attr_u;
};
typedef struct post_op_attr post_op_attr;

struct FSINFO3resok {
	post_op_attr obj_attributes;
	uint32 rtmax;
	uint32 rtpref;
	uint32 rtmult;
	uint32 wtmax;
	uint32 wtpref;
	uint32 wtmult;
	uint32 dtpref;
	size3 maxfilesize;
	nfstime3 time_delta;
	uint32 properties;
};
typedef struct FSINFO3resok FSINFO3resok;

struct FSINFO3resfail {
	post_op_attr obj_attributes;
};
typedef struct FSINFO3resfail FSINFO3resfail;

struct FSINFO3res {
	nfsstat3 status;
	union {
		FSINFO3resok resok;
		FSINFO3resfail resfail;
	} FSINFO3res_u;
};
typedef struct FSINFO3res FSINFO3res;

struct PATHCONF3args {
	nfs_fh3 object;
};
typedef struct PATHCONF3args PATHCONF3args;

struct PATHCONF3resok {
	post_op_attr obj_attributes;
	uint32 linkmax;
	uint32 name_max;
	bool_t no_trunc;
	bool_t chown_restricted;
	bool_t case_insensitive;
	bool_t case_preserving;
};
typedef struct PATHCONF3resok PATHCONF3resok;

struct PATHCONF3resfail {
	post_op_attr obj_attributes;
};
typedef struct PATHCONF3resfail PATHCONF3resfail;

struct PATHCONF3res {
	nfsstat3 status;
	union {
		PATHCONF3resok resok;
		PATHCONF3resfail resfail;
	} PATHCONF3res_u;
};
typedef struct PATHCONF3res PATHCONF3res;

#define MOUNT 100005
#define MOUNT_VERSION 3

#if defined(__STDC__) || defined(__cplusplus)
#define MOUNTPROC3_MNT 1
extern  mountres3 * mountproc3_mnt_3(dirpath *, CLIENT *);
extern  mountres3 * mountproc3_mnt_3_svc(dirpath *, struct svc_req *);
extern int mount_3_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define MOUNTPROC3_MNT 1
extern  mountres3 * mountproc3_mnt_3();
extern  mountres3 * mountproc3_mnt_3_svc();
extern int mount_3_freeresult ();
#endif /* K&R C */

#define NFS_PROGRAM 100003
#define NFS_V3 3

#if defined(__STDC__) || defined(__cplusplus)
#define NFSPROC3_FSINFO 19
extern  FSINFO3res * nfsproc3_fsinfo_3(FSINFO3args *, CLIENT *);
extern  FSINFO3res * nfsproc3_fsinfo_3_svc(FSINFO3args *, struct svc_req *);
#define NFSPROC3_PATHCONF 20
extern  PATHCONF3res * nfsproc3_pathconf_3(PATHCONF3args *, CLIENT *);
extern  PATHCONF3res * nfsproc3_pathconf_3_svc(PATHCONF3args *, struct svc_req *);
extern int nfs_program_3_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define NFSPROC3_FSINFO 19
extern  FSINFO3res * nfsproc3_fsinfo_3();
extern  FSINFO3res * nfsproc3_fsinfo_3_svc();
#define NFSPROC3_PATHCONF 20
extern  PATHCONF3res * nfsproc3_pathconf_3();
extern  PATHCONF3res * nfsproc3_pathconf_3_svc();
extern int nfs_program_3_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_uint64 (XDR *, uint64*);
extern  bool_t xdr_int64 (XDR *, int64*);
extern  bool_t xdr_uint32 (XDR *, uint32*);
extern  bool_t xdr_int32 (XDR *, int32*);
extern  bool_t xdr_filename3 (XDR *, filename3*);
extern  bool_t xdr_nfspath3 (XDR *, nfspath3*);
extern  bool_t xdr_fileid3 (XDR *, fileid3*);
extern  bool_t xdr_cookie3 (XDR *, cookie3*);
extern  bool_t xdr_cookieverf3 (XDR *, cookieverf3);
extern  bool_t xdr_createverf3 (XDR *, createverf3);
extern  bool_t xdr_writeverf3 (XDR *, writeverf3);
extern  bool_t xdr_uid3 (XDR *, uid3*);
extern  bool_t xdr_size3 (XDR *, size3*);
extern  bool_t xdr_gid3 (XDR *, gid3*);
extern  bool_t xdr_offset3 (XDR *, offset3*);
extern  bool_t xdr_mode3 (XDR *, mode3*);
extern  bool_t xdr_count3 (XDR *, count3*);
extern  bool_t xdr_mountstat3 (XDR *, mountstat3*);
extern  bool_t xdr_fhandle3 (XDR *, fhandle3*);
extern  bool_t xdr_dirpath (XDR *, dirpath*);
extern  bool_t xdr_name (XDR *, name*);
extern  bool_t xdr_nfs_fh3 (XDR *, nfs_fh3*);
extern  bool_t xdr_mountres3_ok (XDR *, mountres3_ok*);
extern  bool_t xdr_mountres3 (XDR *, mountres3*);
extern  bool_t xdr_nfsstat3 (XDR *, nfsstat3*);
extern  bool_t xdr_FSINFO3args (XDR *, FSINFO3args*);
extern  bool_t xdr_nfstime3 (XDR *, nfstime3*);
extern  bool_t xdr_specdata3 (XDR *, specdata3*);
extern  bool_t xdr_ftype3 (XDR *, ftype3*);
extern  bool_t xdr_fattr3 (XDR *, fattr3*);
extern  bool_t xdr_post_op_attr (XDR *, post_op_attr*);
extern  bool_t xdr_FSINFO3resok (XDR *, FSINFO3resok*);
extern  bool_t xdr_FSINFO3resfail (XDR *, FSINFO3resfail*);
extern  bool_t xdr_FSINFO3res (XDR *, FSINFO3res*);
extern  bool_t xdr_PATHCONF3args (XDR *, PATHCONF3args*);
extern  bool_t xdr_PATHCONF3resok (XDR *, PATHCONF3resok*);
extern  bool_t xdr_PATHCONF3resfail (XDR *, PATHCONF3resfail*);
extern  bool_t xdr_PATHCONF3res (XDR *, PATHCONF3res*);

#else /* K&R C */
extern bool_t xdr_uint64 ();
extern bool_t xdr_int64 ();
extern bool_t xdr_uint32 ();
extern bool_t xdr_int32 ();
extern bool_t xdr_filename3 ();
extern bool_t xdr_nfspath3 ();
extern bool_t xdr_fileid3 ();
extern bool_t xdr_cookie3 ();
extern bool_t xdr_cookieverf3 ();
extern bool_t xdr_createverf3 ();
extern bool_t xdr_writeverf3 ();
extern bool_t xdr_uid3 ();
extern bool_t xdr_size3 ();
extern bool_t xdr_gid3 ();
extern bool_t xdr_offset3 ();
extern bool_t xdr_mode3 ();
extern bool_t xdr_count3 ();
extern bool_t xdr_mountstat3 ();
extern bool_t xdr_fhandle3 ();
extern bool_t xdr_dirpath ();
extern bool_t xdr_name ();
extern bool_t xdr_nfs_fh3 ();
extern bool_t xdr_mountres3_ok ();
extern bool_t xdr_mountres3 ();
extern bool_t xdr_nfsstat3 ();
extern bool_t xdr_FSINFO3args ();
extern bool_t xdr_nfstime3 ();
extern bool_t xdr_specdata3 ();
extern bool_t xdr_ftype3 ();
extern bool_t xdr_fattr3 ();
extern bool_t xdr_post_op_attr ();
extern bool_t xdr_FSINFO3resok ();
extern bool_t xdr_FSINFO3resfail ();
extern bool_t xdr_FSINFO3res ();
extern bool_t xdr_PATHCONF3args ();
extern bool_t xdr_PATHCONF3resok ();
extern bool_t xdr_PATHCONF3resfail ();
extern bool_t xdr_PATHCONF3res ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_NFS_H_RPCGEN */
