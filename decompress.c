#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "zlib.h"
#define CHUNK_SIZE 4096

char * decompress_data(char * src, int src_size, int * dst_size)
{
    // alloc memory for dst
    char * dst = (char *) calloc (CHUNK_SIZE, 1);
    if (!dst)
    {
        fprintf(stderr, "Not free memory for decompressed data!\n");
        exit(-1);
    }
    int overhead = 1;
    int alreadyWritten = 0;
    int alreadyRead = 0;

    char outbuff[CHUNK_SIZE];
    z_stream stream = { 0 };

    int result = inflateInit(&stream);
    if (result != Z_OK)
    {
    	fprintf(stderr, "inflateInit(...) failed!\n");
        free(dst);
        return 0;
    }

    do {
        int const left_size = src_size - alreadyRead;
        stream.avail_in = (left_size) >= CHUNK_SIZE ? CHUNK_SIZE : left_size;

        if (stream.avail_in <= 0)
            break;

        stream.next_in = src + alreadyRead;
        alreadyRead += stream.avail_in;

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = outbuff;
            result = inflate(&stream, Z_NO_FLUSH);
            if(result == Z_NEED_DICT || result == Z_DATA_ERROR ||
            	result == Z_MEM_ERROR)
            {
            	fprintf(stderr, "inflate(...) failed: %d\n", result);
                inflateEnd(&stream);
                free(dst);
                return 0;
            }

            int nbytes = CHUNK_SIZE - stream.avail_out;

            if (alreadyWritten + nbytes >= CHUNK_SIZE * overhead)
            {
                dst = realloc(dst, CHUNK_SIZE * (++overhead));
                if (!dst)
                {
                    fprintf(stderr, "Not free memory for decompressed data!\n");
                    exit(-1);
                }
            } 
            memcpy(dst + alreadyWritten, outbuff, nbytes);
            alreadyWritten += nbytes;

        } while (stream.avail_out == 0);
    } while (result != Z_STREAM_END);

    inflateEnd(&stream);
    

    dst = realloc(dst, alreadyWritten);
    *dst_size = alreadyWritten;
    return dst;
}
