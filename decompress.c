#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>

#define CHUNK_SIZE 4096

// return size of the written data
int decompress_file(FILE *src, char * dst)
{
    int alreadyWritten = 0;
    uint8_t inbuff[CHUNK_SIZE];
    uint8_t outbuff[CHUNK_SIZE];
    z_stream stream = { 0 };

    int result = inflateInit(&stream);
    if (result != Z_OK)
    {
    	fprintf(stderr, "inflateInit(...) failed!\n");
        return -1;
    }

    do {
        stream.avail_in = fread(inbuff, 1, CHUNK_SIZE, src);
        if (ferror(src))
        {
        	fprintf(stderr, "fread(...) failed!\n");
            inflateEnd(&stream);
            return -1;
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
                return -1;
            }

            uint32_t nbytes = CHUNK_SIZE - stream.avail_out;

            // if (fwrite(outbuff, 1, nbytes, dst) != nbytes || ferror(dst))
            // {
            // 	fprintf(stderr, "fwrite(...) failed!\n");
            //     inflateEnd(&stream);
            //     return -1;
            // }

            dst = memcpy(dst + alreadyWritten, outbuff, nbytes);
            alreadyWritten += nbytes;
        } while (stream.avail_out == 0);
    } while (result != Z_STREAM_END);

    inflateEnd(&stream);

    return alreadyWritten;
}
