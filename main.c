#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>



#include "Error_msg.h"

struct commit_tree
{
    char a; 
};

int isSubstr(char const * where, char const * what);
// Return FILE * pointer to info file and write path to object
FILE * getObject(char const * HASH, char * path_to_object, int isFullHash);
// Return pointer to message begin
char * parserMessage(char const * mesg);
// Rewrite message and return new size
int rewriteMessage(char * allData, char * esgBegin, char * newMesg, int size);


// From decompress.c
int decompress_file(FILE *src, char * dst);
// From compress.c
int compress_data(char * src, int size, FILE *dst);


int main(int argc, char * argv[])
{
    if (argc < 3)
        ERROR(EINVALID_ARG)

    // choose propreate algorithm
    if (!strstr(argv[1], "HEAD"))
    {
        // Get object file
        char editableMSG[4096];
        memset(editableMSG, 0, 4096);
        char path_to_object[256];
        FILE * object = getObject(argv[1], path_to_object, 0);
        if (!object)
            ERROR(EINVALID_FILE);

        // decompress commit info 
        //FILE * editableMSG = fopen("editableMSG.tmp", "wb");
        //if (!editableMSG)
        //    ERROR(EINVALID_FILE);

        int size = decompress_file(object, editableMSG);
        if (!size)
            ERROR("Cann't decompress");

        fclose(object); 

        // Parse msg
        char * msgBegin = parserMessage(editableMSG);

        // Write New msg
        int newSize = rewriteMessage(editableMSG, msgBegin, argv[2], size);
        
        // Composemsg
        FILE * newObject = fopen(path_to_object, "wb");
        if (!newObject)
            ERROR(EINVALID_FILE);

        compress_data(editableMSG, newSize, newObject);
    }
    else
    {
        int depth = 0;
        sscanf(argv[1], "HEAD~%d", &depth);
    }

}


int isSubstr(char const * where, char const * what)
{
    int n = strlen(what);
    for (int i = 0; i != n; ++i)
    {
        if (where[i] != what[i])
            return 0;
    }

    return 1;
}

FILE * getObject(char const * HASH, char * path_to_objects, int isFullHash)
{
    if (strlen(HASH) < 3 || strlen(HASH) > 40)
            ERROR(EINVALID_HASH)

    // parse required hash
    char prefix[3];
    prefix[2] = '\0';
    sscanf(HASH, "%2s", prefix);

    // find required object
    memset(path_to_objects, 0, 256);


    if (isFullHash)
    {
        printf("ASD");
        sprintf(path_to_objects, ".git/objects/%s/%s", prefix, HASH + 2);
        return fopen(path_to_objects, "rb");
    }

    sprintf(path_to_objects, ".git/objects/%s", prefix);

    DIR * dirfd = opendir(path_to_objects);
    if (!dirfd)
        ERROR(EINVALID_FILE);

    struct dirent * dir = (struct dirent *) calloc(1, sizeof(struct dirent));
    while( (dir = readdir(dirfd)) != NULL)
    {
        // if hit
        if (isSubstr(dir->d_name, HASH + 2))
            break;
    }

    if (!dir)
        ERROR(EINVALID_HASH)
    
    // create correct path
    memset(path_to_objects, 0, 256);
    sprintf(path_to_objects, ".git/objects/%2s/%s", prefix, dir->d_name);

    closedir(dirfd);

    // now we get correct zip object
    return fopen(path_to_objects, "rb");
}

char * parserMessage(char const * mesg)
{
    char const * gpgsig = "-----END PGP SIGNATURE-----";
    // +20 for skip '\0'
    char * point_to_gpgsig = strstr(mesg + 20, gpgsig);
    if (point_to_gpgsig)
        point_to_gpgsig += 31; // skip \n\n
    else
        point_to_gpgsig = strstr(mesg + 20, "\n\n") + 2;

    return point_to_gpgsig;
}

int rewriteMessage(char * allData, char * mesgBegin, char * newMesg, int size)
{
    char editableTmp[4096];
    memset(editableTmp, 0, 4096);

    int prefixlen = strlen(allData);
    int messlen = strlen(mesgBegin);
    int newMesglen = strlen(newMesg);
    int datalen = 0;
    sscanf(allData, "commit %d", &datalen);
    
    int newdataSize = datalen - messlen + newMesglen;
    memcpy(mesgBegin, newMesg, newMesglen);

    int writtenBytes = sprintf(editableTmp, "commit %d", newdataSize);
    memcpy(editableTmp + writtenBytes, allData + prefixlen, newdataSize+1);

    int newSize = newdataSize + 1 + writtenBytes;

    memcpy(allData, editableTmp, newSize);

    return newSize;
}

