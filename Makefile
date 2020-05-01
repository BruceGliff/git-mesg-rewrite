all:
	gcc main.c decompress.c compress.c SHAimpl/sha1.c -lz -lcrypto -lssl -o git-mesg-rewrite
