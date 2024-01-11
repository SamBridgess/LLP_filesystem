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

struct GETATTR3args {
	nfs_fh3 object;
};
typedef struct GETATTR3args GETATTR3args;

struct GETATTR3resok {
	fattr3 obj_attributes;
};
typedef struct GETATTR3resok GETATTR3resok;

struct GETATTR3res {
	nfsstat3 status;
	union {
		GETATTR3resok resok;
	} GETATTR3res_u;
};
typedef struct GETATTR3res GETATTR3res;
#define ACCESS3_READ 0x0001
#define ACCESS3_LOOKUP 0x0002
#define ACCESS3_MODIFY 0x0004
#define ACCESS3_EXTEND 0x0008
#define ACCESS3_DELETE 0x0010
#define ACCESS3_EXECUTE 0x0020

struct ACCESS3args {
	nfs_fh3 object;
	uint32 access;
};
typedef struct ACCESS3args ACCESS3args;

struct ACCESS3resok {
	post_op_attr obj_attributes;
	uint32 access;
};
typedef struct ACCESS3resok ACCESS3resok;

struct ACCESS3resfail {
	post_op_attr obj_attributes;
};
typedef struct ACCESS3resfail ACCESS3resfail;

struct ACCESS3res {
	nfsstat3 status;
	union {
		ACCESS3resok resok;
		ACCESS3resfail resfail;
	} ACCESS3res_u;
};
typedef struct ACCESS3res ACCESS3res;

struct READDIR3args {
	nfs_fh3 dir;
	cookie3 cookie;
	cookieverf3 cookieverf;
	count3 count;
};
typedef struct READDIR3args READDIR3args;

struct entry3 {
	fileid3 fileid;
	filename3 name;
	cookie3 cookie;
	struct entry3 *nextentry;
};
typedef struct entry3 entry3;

struct dirlist3 {
	entry3 *entries;
	bool_t eof;
};
typedef struct dirlist3 dirlist3;

struct READDIR3resok {
	post_op_attr dir_attributes;
	cookieverf3 cookieverf;
	dirlist3 reply;
};
typedef struct READDIR3resok READDIR3resok;

struct READDIR3resfail {
	post_op_attr dir_attributes;
};
typedef struct READDIR3resfail READDIR3resfail;

struct READDIR3res {
	nfsstat3 status;
	union {
		READDIR3resok resok;
		READDIR3resfail resfail;
	} READDIR3res_u;
};
typedef struct READDIR3res READDIR3res;

struct diropargs3 {
	nfs_fh3 dir;
	filename3 name;
};
typedef struct diropargs3 diropargs3;

struct LOOKUP3args {
	diropargs3 what;
};
typedef struct LOOKUP3args LOOKUP3args;

struct LOOKUP3resok {
	nfs_fh3 object;
	post_op_attr obj_attributes;
	post_op_attr dir_attributes;
};
typedef struct LOOKUP3resok LOOKUP3resok;

struct LOOKUP3resfail {
	post_op_attr dir_attributes;
};
typedef struct LOOKUP3resfail LOOKUP3resfail;

struct LOOKUP3res {
	nfsstat3 status;
	union {
		LOOKUP3resok resok;
		LOOKUP3resfail resfail;
	} LOOKUP3res_u;
};
typedef struct LOOKUP3res LOOKUP3res;

struct READ3args {
	nfs_fh3 file;
	offset3 offset;
	count3 count;
};
typedef struct READ3args READ3args;

struct READ3resok {
	post_op_attr file_attributes;
	count3 count;
	bool_t eof;
	struct {
		u_int data_len;
		char *data_val;
	} data;
};
typedef struct READ3resok READ3resok;

struct READ3resfail {
	post_op_attr file_attributes;
};
typedef struct READ3resfail READ3resfail;

struct READ3res {
	nfsstat3 status;
	union {
		READ3resok resok;
		READ3resfail resfail;
	} READ3res_u;
};
typedef struct READ3res READ3res;

struct wcc_attr {
	size3 size;
	nfstime3 mtime;
	nfstime3 ctime;
};
typedef struct wcc_attr wcc_attr;

struct pre_op_attr {
	bool_t attributes_follow;
	union {
		wcc_attr attributes;
	} pre_op_attr_u;
};
typedef struct pre_op_attr pre_op_attr;

struct wcc_data {
	pre_op_attr before;
	post_op_attr after;
};
typedef struct wcc_data wcc_data;

enum stable_how {
	UNSTABLE = 0,
	DATA_SYNC = 1,
	FILE_SYNC = 2,
};
typedef enum stable_how stable_how;

struct WRITE3args {
	nfs_fh3 file;
	offset3 offset;
	count3 count;
	stable_how stable;
	struct {
		u_int data_len;
		char *data_val;
	} data;
};
typedef struct WRITE3args WRITE3args;

struct WRITE3resok {
	wcc_data file_wcc;
	count3 count;
	stable_how committed;
	writeverf3 verf;
};
typedef struct WRITE3resok WRITE3resok;

struct WRITE3resfail {
	wcc_data file_wcc;
};
typedef struct WRITE3resfail WRITE3resfail;

struct WRITE3res {
	nfsstat3 status;
	union {
		WRITE3resok resok;
		WRITE3resfail resfail;
	} WRITE3res_u;
};
typedef struct WRITE3res WRITE3res;

enum time_how {
	DONT_CHANGE = 0,
	SET_TO_SERVER_TIME = 1,
	SET_TO_CLIENT_TIME = 2,
};
typedef enum time_how time_how;

struct set_mode3 {
	bool_t set_it;
	union {
		mode3 mode;
	} set_mode3_u;
};
typedef struct set_mode3 set_mode3;

struct set_uid3 {
	bool_t set_it;
	union {
		uid3 uid;
	} set_uid3_u;
};
typedef struct set_uid3 set_uid3;

struct set_gid3 {
	bool_t set_it;
	union {
		gid3 gid;
	} set_gid3_u;
};
typedef struct set_gid3 set_gid3;

struct set_size3 {
	bool_t set_it;
	union {
		size3 size;
	} set_size3_u;
};
typedef struct set_size3 set_size3;

struct set_atime {
	time_how set_it;
	union {
		nfstime3 atime;
	} set_atime_u;
};
typedef struct set_atime set_atime;

struct set_mtime {
	time_how set_it;
	union {
		nfstime3 mtime;
	} set_mtime_u;
};
typedef struct set_mtime set_mtime;

struct sattr3 {
	set_mode3 mode;
	set_uid3 uid;
	set_gid3 gid;
	set_size3 size;
	set_atime atime;
	set_mtime mtime;
};
typedef struct sattr3 sattr3;

struct post_op_fh3 {
	bool_t handle_follows;
	union {
		nfs_fh3 handle;
	} post_op_fh3_u;
};
typedef struct post_op_fh3 post_op_fh3;

enum createmode3 {
	UNCHECKED = 0,
	GUARDED = 1,
	EXCLUSIVE = 2,
};
typedef enum createmode3 createmode3;

struct createhow3 {
	createmode3 mode;
	union {
		sattr3 obj_attributes;
		createverf3 verf;
	} createhow3_u;
};
typedef struct createhow3 createhow3;

struct CREATE3args {
	diropargs3 where;
	createhow3 how;
};
typedef struct CREATE3args CREATE3args;

struct CREATE3resok {
	post_op_fh3 obj;
	post_op_attr obj_attributes;
	wcc_data dir_wcc;
};
typedef struct CREATE3resok CREATE3resok;

struct CREATE3resfail {
	wcc_data dir_wcc;
};
typedef struct CREATE3resfail CREATE3resfail;

struct CREATE3res {
	nfsstat3 status;
	union {
		CREATE3resok resok;
		CREATE3resfail resfail;
	} CREATE3res_u;
};
typedef struct CREATE3res CREATE3res;

struct FSSTAT3args {
	nfs_fh3 fsroot;
};
typedef struct FSSTAT3args FSSTAT3args;

struct FSSTAT3resok {
	post_op_attr obj_attributes;
	size3 tbytes;
	size3 fbytes;
	size3 abytes;
	size3 tfiles;
	size3 ffiles;
	size3 afiles;
	uint32 invarsec;
};
typedef struct FSSTAT3resok FSSTAT3resok;

struct FSSTAT3resfail {
	post_op_attr obj_attributes;
};
typedef struct FSSTAT3resfail FSSTAT3resfail;

struct FSSTAT3res {
	nfsstat3 status;
	union {
		FSSTAT3resok resok;
		FSSTAT3resfail resfail;
	} FSSTAT3res_u;
};
typedef struct FSSTAT3res FSSTAT3res;

struct sattrguard3 {
	bool_t check;
	union {
		nfstime3 obj_ctime;
	} sattrguard3_u;
};
typedef struct sattrguard3 sattrguard3;

struct SETATTR3args {
	nfs_fh3 object;
	sattr3 new_attributes;
	sattrguard3 guard;
};
typedef struct SETATTR3args SETATTR3args;

struct SETATTR3resok {
	wcc_data obj_wcc;
};
typedef struct SETATTR3resok SETATTR3resok;

struct SETATTR3resfail {
	wcc_data obj_wcc;
};
typedef struct SETATTR3resfail SETATTR3resfail;

struct SETATTR3res {
	nfsstat3 status;
	union {
		SETATTR3resok resok;
		SETATTR3resfail resfail;
	} SETATTR3res_u;
};
typedef struct SETATTR3res SETATTR3res;

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
#define NFSPROC3_GETATTR 1
extern  GETATTR3res * nfsproc3_getattr_3(GETATTR3args *, CLIENT *);
extern  GETATTR3res * nfsproc3_getattr_3_svc(GETATTR3args *, struct svc_req *);
#define NFSPROC3_SETATTR 2
extern  SETATTR3res * nfsproc3_setattr_3(SETATTR3args *, CLIENT *);
extern  SETATTR3res * nfsproc3_setattr_3_svc(SETATTR3args *, struct svc_req *);
#define NFSPROC3_LOOKUP 3
extern  LOOKUP3res * nfsproc3_lookup_3(LOOKUP3args *, CLIENT *);
extern  LOOKUP3res * nfsproc3_lookup_3_svc(LOOKUP3args *, struct svc_req *);
#define NFSPROC3_ACCESS 4
extern  ACCESS3res * nfsproc3_access_3(ACCESS3args *, CLIENT *);
extern  ACCESS3res * nfsproc3_access_3_svc(ACCESS3args *, struct svc_req *);
#define NFSPROC3_READ 6
extern  READ3res * nfsproc3_read_3(READ3args *, CLIENT *);
extern  READ3res * nfsproc3_read_3_svc(READ3args *, struct svc_req *);
#define NFSPROC3_WRITE 7
extern  WRITE3res * nfsproc3_write_3(WRITE3args *, CLIENT *);
extern  WRITE3res * nfsproc3_write_3_svc(WRITE3args *, struct svc_req *);
#define NFSPROC3_CREATE 8
extern  CREATE3res * nfsproc3_create_3(CREATE3args *, CLIENT *);
extern  CREATE3res * nfsproc3_create_3_svc(CREATE3args *, struct svc_req *);
#define NFSPROC3_READDIR 16
extern  READDIR3res * nfsproc3_readdir_3(READDIR3args *, CLIENT *);
extern  READDIR3res * nfsproc3_readdir_3_svc(READDIR3args *, struct svc_req *);
#define NFSPROC3_FSSTAT 18
extern  FSSTAT3res * nfsproc3_fsstat_3(FSSTAT3args *, CLIENT *);
extern  FSSTAT3res * nfsproc3_fsstat_3_svc(FSSTAT3args *, struct svc_req *);
#define NFSPROC3_FSINFO 19
extern  FSINFO3res * nfsproc3_fsinfo_3(FSINFO3args *, CLIENT *);
extern  FSINFO3res * nfsproc3_fsinfo_3_svc(FSINFO3args *, struct svc_req *);
#define NFSPROC3_PATHCONF 20
extern  PATHCONF3res * nfsproc3_pathconf_3(PATHCONF3args *, CLIENT *);
extern  PATHCONF3res * nfsproc3_pathconf_3_svc(PATHCONF3args *, struct svc_req *);
extern int nfs_program_3_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define NFSPROC3_GETATTR 1
extern  GETATTR3res * nfsproc3_getattr_3();
extern  GETATTR3res * nfsproc3_getattr_3_svc();
#define NFSPROC3_SETATTR 2
extern  SETATTR3res * nfsproc3_setattr_3();
extern  SETATTR3res * nfsproc3_setattr_3_svc();
#define NFSPROC3_LOOKUP 3
extern  LOOKUP3res * nfsproc3_lookup_3();
extern  LOOKUP3res * nfsproc3_lookup_3_svc();
#define NFSPROC3_ACCESS 4
extern  ACCESS3res * nfsproc3_access_3();
extern  ACCESS3res * nfsproc3_access_3_svc();
#define NFSPROC3_READ 6
extern  READ3res * nfsproc3_read_3();
extern  READ3res * nfsproc3_read_3_svc();
#define NFSPROC3_WRITE 7
extern  WRITE3res * nfsproc3_write_3();
extern  WRITE3res * nfsproc3_write_3_svc();
#define NFSPROC3_CREATE 8
extern  CREATE3res * nfsproc3_create_3();
extern  CREATE3res * nfsproc3_create_3_svc();
#define NFSPROC3_READDIR 16
extern  READDIR3res * nfsproc3_readdir_3();
extern  READDIR3res * nfsproc3_readdir_3_svc();
#define NFSPROC3_FSSTAT 18
extern  FSSTAT3res * nfsproc3_fsstat_3();
extern  FSSTAT3res * nfsproc3_fsstat_3_svc();
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
extern  bool_t xdr_GETATTR3args (XDR *, GETATTR3args*);
extern  bool_t xdr_GETATTR3resok (XDR *, GETATTR3resok*);
extern  bool_t xdr_GETATTR3res (XDR *, GETATTR3res*);
extern  bool_t xdr_ACCESS3args (XDR *, ACCESS3args*);
extern  bool_t xdr_ACCESS3resok (XDR *, ACCESS3resok*);
extern  bool_t xdr_ACCESS3resfail (XDR *, ACCESS3resfail*);
extern  bool_t xdr_ACCESS3res (XDR *, ACCESS3res*);
extern  bool_t xdr_READDIR3args (XDR *, READDIR3args*);
extern  bool_t xdr_entry3 (XDR *, entry3*);
extern  bool_t xdr_dirlist3 (XDR *, dirlist3*);
extern  bool_t xdr_READDIR3resok (XDR *, READDIR3resok*);
extern  bool_t xdr_READDIR3resfail (XDR *, READDIR3resfail*);
extern  bool_t xdr_READDIR3res (XDR *, READDIR3res*);
extern  bool_t xdr_diropargs3 (XDR *, diropargs3*);
extern  bool_t xdr_LOOKUP3args (XDR *, LOOKUP3args*);
extern  bool_t xdr_LOOKUP3resok (XDR *, LOOKUP3resok*);
extern  bool_t xdr_LOOKUP3resfail (XDR *, LOOKUP3resfail*);
extern  bool_t xdr_LOOKUP3res (XDR *, LOOKUP3res*);
extern  bool_t xdr_READ3args (XDR *, READ3args*);
extern  bool_t xdr_READ3resok (XDR *, READ3resok*);
extern  bool_t xdr_READ3resfail (XDR *, READ3resfail*);
extern  bool_t xdr_READ3res (XDR *, READ3res*);
extern  bool_t xdr_wcc_attr (XDR *, wcc_attr*);
extern  bool_t xdr_pre_op_attr (XDR *, pre_op_attr*);
extern  bool_t xdr_wcc_data (XDR *, wcc_data*);
extern  bool_t xdr_stable_how (XDR *, stable_how*);
extern  bool_t xdr_WRITE3args (XDR *, WRITE3args*);
extern  bool_t xdr_WRITE3resok (XDR *, WRITE3resok*);
extern  bool_t xdr_WRITE3resfail (XDR *, WRITE3resfail*);
extern  bool_t xdr_WRITE3res (XDR *, WRITE3res*);
extern  bool_t xdr_time_how (XDR *, time_how*);
extern  bool_t xdr_set_mode3 (XDR *, set_mode3*);
extern  bool_t xdr_set_uid3 (XDR *, set_uid3*);
extern  bool_t xdr_set_gid3 (XDR *, set_gid3*);
extern  bool_t xdr_set_size3 (XDR *, set_size3*);
extern  bool_t xdr_set_atime (XDR *, set_atime*);
extern  bool_t xdr_set_mtime (XDR *, set_mtime*);
extern  bool_t xdr_sattr3 (XDR *, sattr3*);
extern  bool_t xdr_post_op_fh3 (XDR *, post_op_fh3*);
extern  bool_t xdr_createmode3 (XDR *, createmode3*);
extern  bool_t xdr_createhow3 (XDR *, createhow3*);
extern  bool_t xdr_CREATE3args (XDR *, CREATE3args*);
extern  bool_t xdr_CREATE3resok (XDR *, CREATE3resok*);
extern  bool_t xdr_CREATE3resfail (XDR *, CREATE3resfail*);
extern  bool_t xdr_CREATE3res (XDR *, CREATE3res*);
extern  bool_t xdr_FSSTAT3args (XDR *, FSSTAT3args*);
extern  bool_t xdr_FSSTAT3resok (XDR *, FSSTAT3resok*);
extern  bool_t xdr_FSSTAT3resfail (XDR *, FSSTAT3resfail*);
extern  bool_t xdr_FSSTAT3res (XDR *, FSSTAT3res*);
extern  bool_t xdr_sattrguard3 (XDR *, sattrguard3*);
extern  bool_t xdr_SETATTR3args (XDR *, SETATTR3args*);
extern  bool_t xdr_SETATTR3resok (XDR *, SETATTR3resok*);
extern  bool_t xdr_SETATTR3resfail (XDR *, SETATTR3resfail*);
extern  bool_t xdr_SETATTR3res (XDR *, SETATTR3res*);

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
extern bool_t xdr_GETATTR3args ();
extern bool_t xdr_GETATTR3resok ();
extern bool_t xdr_GETATTR3res ();
extern bool_t xdr_ACCESS3args ();
extern bool_t xdr_ACCESS3resok ();
extern bool_t xdr_ACCESS3resfail ();
extern bool_t xdr_ACCESS3res ();
extern bool_t xdr_READDIR3args ();
extern bool_t xdr_entry3 ();
extern bool_t xdr_dirlist3 ();
extern bool_t xdr_READDIR3resok ();
extern bool_t xdr_READDIR3resfail ();
extern bool_t xdr_READDIR3res ();
extern bool_t xdr_diropargs3 ();
extern bool_t xdr_LOOKUP3args ();
extern bool_t xdr_LOOKUP3resok ();
extern bool_t xdr_LOOKUP3resfail ();
extern bool_t xdr_LOOKUP3res ();
extern bool_t xdr_READ3args ();
extern bool_t xdr_READ3resok ();
extern bool_t xdr_READ3resfail ();
extern bool_t xdr_READ3res ();
extern bool_t xdr_wcc_attr ();
extern bool_t xdr_pre_op_attr ();
extern bool_t xdr_wcc_data ();
extern bool_t xdr_stable_how ();
extern bool_t xdr_WRITE3args ();
extern bool_t xdr_WRITE3resok ();
extern bool_t xdr_WRITE3resfail ();
extern bool_t xdr_WRITE3res ();
extern bool_t xdr_time_how ();
extern bool_t xdr_set_mode3 ();
extern bool_t xdr_set_uid3 ();
extern bool_t xdr_set_gid3 ();
extern bool_t xdr_set_size3 ();
extern bool_t xdr_set_atime ();
extern bool_t xdr_set_mtime ();
extern bool_t xdr_sattr3 ();
extern bool_t xdr_post_op_fh3 ();
extern bool_t xdr_createmode3 ();
extern bool_t xdr_createhow3 ();
extern bool_t xdr_CREATE3args ();
extern bool_t xdr_CREATE3resok ();
extern bool_t xdr_CREATE3resfail ();
extern bool_t xdr_CREATE3res ();
extern bool_t xdr_FSSTAT3args ();
extern bool_t xdr_FSSTAT3resok ();
extern bool_t xdr_FSSTAT3resfail ();
extern bool_t xdr_FSSTAT3res ();
extern bool_t xdr_sattrguard3 ();
extern bool_t xdr_SETATTR3args ();
extern bool_t xdr_SETATTR3resok ();
extern bool_t xdr_SETATTR3resfail ();
extern bool_t xdr_SETATTR3res ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_NFS_H_RPCGEN */
