all:
	gcc -g -o nfs_server stuff.c read.c nfs_server.c nfs_svc.c nfs_xdr.c  -lnsl
gen:
	rpcgen -a -C nfs.x
	rm Makefile.nfs nfs_client.c nfs_clnt.c
clean:
	rm nfs_server