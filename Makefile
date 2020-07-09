all : git-mesg-rewrite.out

git-mesg-rewrite.out : main.o decompress.o compress.o message.o commit_tree.o sha1.o
	cc $(CFLAGS) -o $@ $^ $(LIBFLAGS)

sha1.o :
	cc $(CFLAGS) -c -o sha1.o SHAimpl/sha1.c
CFLAGS = -O3
LIBFLAGS = -lz -lcrypto -lssl

clean:
	rm *.o *.out
