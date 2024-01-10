#include "read.h"

int open_file(const char *file_path) {
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
    }
    return fd;
}

ssize_t read_file_chunk(int fd, off_t offset, size_t chunk_size, char *buffer) {
    ssize_t bytes_read = pread(fd, buffer, chunk_size, offset);
    if (bytes_read == -1) {
        perror("Error reading file");
    }
    return bytes_read;
}

void fill_READ3res_stream(const char *file_path, off_t offset, size_t count, READ3res *result) {
    int fd = open_file(file_path);
    if (fd == -1) {
        result->status = NFS3ERR_IO;
        return;
    }
    printf("Opening file: %s, offset: %lld, count: %zu\n", file_path, (long long)offset, count);

    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking in file");
        close(fd);
        result->status = NFS3ERR_IO;
        return;
    }

    char *data_buffer = (char *)malloc(count);
    if (!data_buffer) {
        perror("Error allocating memory");
        close(fd);
        result->status = NFS3ERR_IO;
        return;
    }

    ssize_t bytes_read = read_file_chunk(fd, offset, count, data_buffer);

    if (bytes_read > 0) {
        result->status = NFS3_OK;
        result->READ3res_u.resok.count = bytes_read;
        result->READ3res_u.resok.eof = 0;

        result->READ3res_u.resok.data.data_len = bytes_read;
        //result->READ3res_u.resok.data.data_val = data_buffer;
        result->READ3res_u.resok.data.data_val = (char *)malloc(bytes_read);
        if (result->READ3res_u.resok.data.data_val) {
            memcpy(result->READ3res_u.resok.data.data_val, data_buffer, bytes_read);
        } else {
            perror("Error allocating memory for data");
            result->status = NFS3ERR_IO;
        }
    } else if (bytes_read == 0) {
        result->status = NFS3_OK;
        result->READ3res_u.resok.count = 0;
        result->READ3res_u.resok.eof = 1;
        result->READ3res_u.resok.data.data_len = 0;
        result->READ3res_u.resok.data.data_val = NULL;
    } else {
        result->status = NFS3ERR_IO;
    }


    close(fd);
    free(data_buffer);
}