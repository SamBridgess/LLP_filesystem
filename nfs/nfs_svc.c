/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "nfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
mount_3(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		dirpath mountproc3_mnt_3_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case MOUNTPROC3_MNT:
		_xdr_argument = (xdrproc_t) xdr_dirpath;
		_xdr_result = (xdrproc_t) xdr_mountres3;
		local = (char *(*)(char *, struct svc_req *)) mountproc3_mnt_3_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

static void
nfs_program_3(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		GETATTR3args nfsproc3_getattr_3_arg;
		LOOKUP3args nfsproc3_lookup_3_arg;
		ACCESS3args nfsproc3_access_3_arg;
		READ3args nfsproc3_read_3_arg;
		READDIR3args nfsproc3_readdir_3_arg;
		FSINFO3args nfsproc3_fsinfo_3_arg;
		PATHCONF3args nfsproc3_pathconf_3_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case NFSPROC3_GETATTR:
		_xdr_argument = (xdrproc_t) xdr_GETATTR3args;
		_xdr_result = (xdrproc_t) xdr_GETATTR3res;
		local = (char *(*)(char *, struct svc_req *)) nfsproc3_getattr_3_svc;
		break;

	case NFSPROC3_LOOKUP:
		_xdr_argument = (xdrproc_t) xdr_LOOKUP3args;
		_xdr_result = (xdrproc_t) xdr_LOOKUP3res;
		local = (char *(*)(char *, struct svc_req *)) nfsproc3_lookup_3_svc;
		break;

	case NFSPROC3_ACCESS:
		_xdr_argument = (xdrproc_t) xdr_ACCESS3args;
		_xdr_result = (xdrproc_t) xdr_ACCESS3res;
		local = (char *(*)(char *, struct svc_req *)) nfsproc3_access_3_svc;
		break;

	case NFSPROC3_READ:
		_xdr_argument = (xdrproc_t) xdr_READ3args;
		_xdr_result = (xdrproc_t) xdr_READ3res;
		local = (char *(*)(char *, struct svc_req *)) nfsproc3_read_3_svc;
		break;

	case NFSPROC3_READDIR:
		_xdr_argument = (xdrproc_t) xdr_READDIR3args;
		_xdr_result = (xdrproc_t) xdr_READDIR3res;
		local = (char *(*)(char *, struct svc_req *)) nfsproc3_readdir_3_svc;
		break;

	case NFSPROC3_FSINFO:
		_xdr_argument = (xdrproc_t) xdr_FSINFO3args;
		_xdr_result = (xdrproc_t) xdr_FSINFO3res;
		local = (char *(*)(char *, struct svc_req *)) nfsproc3_fsinfo_3_svc;
		break;

	case NFSPROC3_PATHCONF:
		_xdr_argument = (xdrproc_t) xdr_PATHCONF3args;
		_xdr_result = (xdrproc_t) xdr_PATHCONF3res;
		local = (char *(*)(char *, struct svc_req *)) nfsproc3_pathconf_3_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (MOUNT, MOUNT_VERSION);
	pmap_unset (NFS_PROGRAM, NFS_V3);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, MOUNT, MOUNT_VERSION, mount_3, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (MOUNT, MOUNT_VERSION, udp).");
		exit(1);
	}
	if (!svc_register(transp, NFS_PROGRAM, NFS_V3, nfs_program_3, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (NFS_PROGRAM, NFS_V3, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, MOUNT, MOUNT_VERSION, mount_3, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (MOUNT, MOUNT_VERSION, tcp).");
		exit(1);
	}
	if (!svc_register(transp, NFS_PROGRAM, NFS_V3, nfs_program_3, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (NFS_PROGRAM, NFS_V3, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
