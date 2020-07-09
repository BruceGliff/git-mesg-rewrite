all : git-mesg-rewrite.out

git-mesg-rewrite.out : main.o decompress.o compress.o message.o commit_tree.o sha1.o
	gcc -o $@ $^

sha1.o :
	gcc -o SHAimpl/sha1.c
CFLAGS = -O3 -lz -lcrypto -lssl

clean:
	rm *.o *.out
