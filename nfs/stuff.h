#ifndef LLP_FILESYSTEM_STUFF_H
#define LLP_FILESYSTEM_STUFF_H
#include "nfs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#define NFS_MAXDATA_TCP 524288
#define NFS_MAXDATA_UDP 32768

#define backend_fsinfo_properties FSF3_LINK | FSF3_SYMLINK | FSF3_HOMOGENEOUS | FSF3_CANSETTIME;

#define FH_MINLEN 21
#define FH_MAXBUF 64
#define FH_MAXLEN (FH_MAXBUF - FH_MINLEN)
#define NFS_MAXPATHLEN 1024
#define CACHE_ENTRIES    4096
typedef struct {
    uint32 dev;			/* device */
    uint64 ino;			/* inode */
    char path[NFS_MAXPATHLEN];	/* pathname */
    unsigned int use;		/* last use */
} unfs3_cache_t;
static unfs3_cache_t fh_cache[CACHE_ENTRIES];

typedef struct {
    uint32			dev;
    uint64			ino;
    uint32			gen;
    uint32          pwhash;
    unsigned char	len;
    unsigned char	inos[FH_MAXLEN];
} unfs3_fh_t;


int get_socket_type(struct svc_req *rqstp);
u_int fh_length(const unfs3_fh_t * fh);
nfs_fh3 fh_encode(const unfs3_fh_t *fh, char *buffer);

#endif //LLP_FILESYSTEM_STUFF_H
