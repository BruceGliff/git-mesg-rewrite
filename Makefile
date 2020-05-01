all:
	gcc main.c decompress.c compress.c -lz -o git-mesg-rewrite
