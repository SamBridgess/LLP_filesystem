# NFS V3
Simple read-only NFS V3

## Building NFS
Build
```
make
```

Generate `nfs_server.c`, `nfs_svc.c`, `nfs_sdr.c` and `nfs.h` from `nfs.x`. <br>
BE CAREFUL, it rewrites your current files!
```
make gen
```
Clean
```
make clean
```

## Commands for mounting this NFS implementation

Mount:
```
sudo mount -t nfs -o vers=3,nordirplus  127.0.0.1:/path/to/remote/directory /local/direcory -v
```

Unmount:
```
sudo umount nfs-mount
```
In case this does not work, try adding `-l` option to `umount` command


