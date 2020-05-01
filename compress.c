#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>

#define CHUNK_SIZE 4096
#define COMPRESSION_LEVEL 9

int compress_data(char * src, int size, FILE *dst)
{
    uint8_t inbuff[CHUNK_SIZE];
    uint8_t outbuff[CHUNK_SIZE];
    z_stream stream = {0};

    if (deflateInit(&stream, COMPRESSION_LEVEL) != Z_OK)
    {
    	fprintf(stderr, "deflateInit(...) failed!\n");
        return -1;
    }

    stream.avail_in = size;
    memcpy(inbuff, src, size);

    int flush = Z_FINISH;
    stream.next_in = inbuff;

    do {
        stream.avail_out = CHUNK_SIZE;
        stream.next_out = outbuff;
        deflate(&stream, flush);
        uint32_t nbytes = CHUNK_SIZE - stream.avail_out;

        if (fwrite(outbuff, 1, nbytes, dst) != nbytes || ferror(dst))
        {
            fprintf(stderr, "fwrite(...) failed!\n");
            deflateEnd(&stream);
            return -1;
        }
    } while (stream.avail_out == 0);

    deflateEnd(&stream);
    return 0;
}