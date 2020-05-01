#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

int toSHA1(char const * buff, int size, char * out)
{
    int i = 0;
    unsigned char result[SHA_DIGEST_LENGTH];

    SHA1(buff, size, result);

    for(i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf(out+i * 2, "%02x", result[i]);

    return 0;
}