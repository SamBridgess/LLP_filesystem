#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void usage() {
	printf("Please provide a pathname and size as input.\n");
	printf("Example usage:\n");
	printf("\t./trunc testdir/to_trunc.txt 1000\n");
}

int do_truncate(char *path, off_t new_size) {
	int ret;
	ret = truncate(path, new_size);
	if (ret < 0) {
		perror("truncate error:");
		return errno;
	}
	return ret;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		usage();
		exit(errno);
	}

	return do_truncate(argv[1], atoi(argv[2]));
}
