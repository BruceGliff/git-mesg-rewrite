all:
	gcc main.c decompress.c compress.c message.c commit_tree.c SHAimpl/sha1.c -lz -lcrypto -lssl -o git-mesg-rewrite
