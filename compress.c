#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "zlib.h"

#define CHUNK_SIZE 4096
#define COMPRESSION_LEVEL 9

char * compress_data(char * src, int src_size, int * dst_size)
{
    // alloc memory for dst
    char * dst = (char *) calloc (CHUNK_SIZE, 1);
    if (!dst)
    {
        fprintf(stderr, "Not free memory for compressed data!\n");
        exit(-1);
    }
    int overhead = 1;
    int alreadyWritten = 0;
    int alreadyRead = 0;

    char outbuff[CHUNK_SIZE];
    z_stream stream = {0};

    if (deflateInit(&stream, COMPRESSION_LEVEL) != Z_OK)
    {
    	fprintf(stderr, "deflateInit(...) failed!\n");
        free(dst);
        return 0;
    }

	int flush = 0;
    do {
        int const left_size = src_size - alreadyRead;
        stream.avail_in = (left_size >= CHUNK_SIZE) ? CHUNK_SIZE : left_size;
        flush = (left_size < CHUNK_SIZE) ? Z_FINISH : Z_NO_FLUSH;
        stream.next_in = src + alreadyRead;
        alreadyRead += stream.avail_in;

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = outbuff;
            deflate(&stream, flush);
            int nbytes = CHUNK_SIZE - stream.avail_out;

            if (alreadyWritten + nbytes >= CHUNK_SIZE * overhead)
            {
                dst = realloc(dst, CHUNK_SIZE * (++overhead));
                if (!dst)
                {
                    fprintf(stderr, "Not free memory for compressed data!\n");
                    exit(-1);
                }
            }
                
            memcpy(dst + alreadyWritten, outbuff, nbytes);
            alreadyWritten += nbytes;

        } while (stream.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&stream);
    

    dst = realloc(dst, alreadyWritten);
    *dst_size = alreadyWritten;
    return dst;
}
