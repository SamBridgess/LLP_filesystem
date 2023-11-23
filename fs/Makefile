EXEC = artifacts/fat
SRC = fat.c

CC	=	gcc
CFLAGS	=	-g -Og -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wconversion $(PKGFLAGS)

PKGFLAGS	=	`pkg-config fuse --cflags --libs`

.PHONY: all clean

all:
	mkdir -p artifacts
	$(CC) -o $(EXEC) $(SRC) $(CFLAGS)

run:
	mkdir -p mount_test
	./artifacts/fat -s mount_test

unmount:
	fusermount -u mount_test

clean:
	make unmount
	rm artifacts/*
	rmdir artifacts

