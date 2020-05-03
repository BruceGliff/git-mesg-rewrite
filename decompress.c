#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>

#define CHUNK_SIZE 4096

// allocated and written data and size in message_size
char * decompress_file(int src_fd, int * message_size)
{
    char * dst = (char *) calloc (CHUNK_SIZE, 1);
    int overhead = 1;
    int alreadyWritten = 0;
    uint8_t inbuff[CHUNK_SIZE];
    uint8_t outbuff[CHUNK_SIZE];
    z_stream stream = { 0 };

    int result = inflateInit(&stream);
    if (result != Z_OK)
    {
    	fprintf(stderr, "inflateInit(...) failed!\n");
        return 0;
    }

    do {
        stream.avail_in = read(src_fd, inbuff, CHUNK_SIZE);
        if (stream.avail_in == -1)
        {
        	fprintf(stderr, "read(...) failed!\n");
            inflateEnd(&stream);
            return 0;
        }

        if (stream.avail_in == 0)
            break;

        stream.next_in = inbuff;
        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = outbuff;
            result = inflate(&stream, Z_NO_FLUSH);
            if(result == Z_NEED_DICT || result == Z_DATA_ERROR ||
            	result == Z_MEM_ERROR)
            {
            	fprintf(stderr, "inflate(...) failed: %d\n", result);
                inflateEnd(&stream);
                return 0;
            }

            uint32_t nbytes = CHUNK_SIZE - stream.avail_out;

            if (alreadyWritten + nbytes >= CHUNK_SIZE)
            {
                char * tmp = (char *) calloc (CHUNK_SIZE * (++overhead), 1);
                memcpy(tmp, dst, alreadyWritten);
                free(dst);
                dst = tmp;
            }

            dst = memcpy(dst + alreadyWritten, outbuff, nbytes);
            alreadyWritten += nbytes;
        } while (stream.avail_out == 0);
    } while (result != Z_STREAM_END);

    inflateEnd(&stream);

    dst = realloc(dst, alreadyWritten);
    *message_size = alreadyWritten;
    return dst;
}
