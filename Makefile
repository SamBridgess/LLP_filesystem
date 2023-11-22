EXEC = fat
SRC = fat.c

CC	=	gcc
CFLAGS	=	-g -Og -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wconversion $(PKGFLAGS)

PKGFLAGS	=	`pkg-config fuse --cflags --libs`

.PHONY: all clean

all:
	$(CC) -o $(EXEC) $(SRC) $(CFLAGS)

clean:
	rm $(EXEC)
	rm fat_disk
remake:
	fusermount -u Test2
	make
	./fat -s Test2
all_at_once:
	fusermount -u Test2
	make clean & make
	./fat -s Test2
start:
	make
	./fat -s Test2


