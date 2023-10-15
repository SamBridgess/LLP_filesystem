default: build
build: 
	gcc llp_fs.c -o llp_fs `pkg-config fuse --cflags --libs`

run:
	mkdir -p mount_point
	./llp_fs -f ./mount_point
clean:
	rm llp_fs
	rm -r mount_point