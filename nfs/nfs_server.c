/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "nfs.h"

mountres3 *
mountproc3_mnt_3_svc(dirpath *argp, struct svc_req *rqstp)
{
    printf("!!\n");
    static mountres3  result;


    static int auth = AUTH_UNIX;

    static char nonce[32] = "01234567890123456789012345678901";

    result.fhs_status = MNT3_OK;
    result.mountres3_u.mountinfo.fhandle.fhandle3_len = 32;
    result.mountres3_u.mountinfo.fhandle.fhandle3_val = nonce;
    result.mountres3_u.mountinfo.auth_flavors.auth_flavors_len = 1;
    result.mountres3_u.mountinfo.auth_flavors.auth_flavors_val = &auth;

	/*
	 * insert server code here
	 */

	return &result;
}

char **
read_3_svc(char **argp, struct svc_req *rqstp)
{
	static char * result;

	/*
	 * insert server code here
	 */

	return &result;
}