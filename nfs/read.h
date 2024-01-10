#ifndef LLP_FILESYSTEM_READ_H
#define LLP_FILESYSTEM_READ_H

#include "nfs.h"
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>


int open_file(const char *file_path);
ssize_t read_file_chunk(int fd, off_t offset, size_t chunk_size, char *buffer);
void fill_READ3res_stream(const char *file_path, off_t offset, size_t count, READ3res *result);


#endif //LLP_FILESYSTEM_READ_H
