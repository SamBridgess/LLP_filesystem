#include "nfs.h"
#include <assert.h>
#include "stuff.h"


int get_socket_type(struct svc_req *rqstp) {
    int v, res;
    socklen_t l;
    l = sizeof(v);
    res = getsockopt(1, SOL_SOCKET, SO_TYPE, &v, &l);
    if (res < 0) return -1;
    return v;
}


u_int fh_length(const unfs3_fh_t * fh) {
    return fh->len + sizeof(fh->len) + sizeof(fh->dev) + sizeof(fh->ino) +
           sizeof(fh->gen) + sizeof(fh->pwhash);
}


//Convert a unfs3_fh_t to a nfs_fh3
nfs_fh3 fh_encode(const unfs3_fh_t *fh, char *buffer) {
    nfs_fh3 handle;
    char *buf;

    assert(fh);
    assert(buffer);

    handle.data.data_len = fh_length(fh);
    handle.data.data_val = buffer;

    buf = buffer;

    memcpy(buf, &fh->dev, sizeof(fh->dev));
    buf += sizeof(fh->dev);
    memcpy(buf, &fh->ino, sizeof(fh->ino));
    buf += sizeof(fh->ino);
    memcpy(buf, &fh->gen, sizeof(fh->gen));
    buf += sizeof(fh->gen);
    memcpy(buf, &fh->pwhash, sizeof(fh->pwhash));
    buf += sizeof(fh->pwhash);
    memcpy(buf, &fh->len, sizeof(fh->len));
    buf += sizeof(fh->len);
    if (fh->len)
        memcpy(buf, fh->inos, fh->len);

    return handle;
}
