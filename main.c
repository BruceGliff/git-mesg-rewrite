#include <stdio.h>
#include <string.h>



#include "Error_msg.h"

struct commit_tree
{
    char a; 
};


int main(int argc, char * argv[])
{
    if (argc < 3)
        ERROR(EINVALID_ARG)

    // choose propreate algorithm
    if (!strstr(argv[1], "HEAD"))
    {
        if (strlen(argv[1]) < 3 || strlen(argv[1]) > 40)
            ERROR(EINVALID_HASH)

        // parse required hash
        char prefix[2];
        sscanf(argv[1], "%2s", prefix);

        // find required object
        char path_to_objects[256];
        memset(path_to_objects, 0, 256);
        sprintf(path_to_objects, ".git/object/%s/%s*", prefix, argv[1]+2);

        FILE * object = fopen(path_to_objects, "rb");
        if (!object)
            ERROR(EINVALID_FILE)
    }
    else
    {
        int depth = 0;
        sscanf(argv[1], "HEAD~%d", &depth);
    }


    // The required commit

    // get necessary commit tree // deep in until goot commit
    // get propreate commit hash
    // get all info from object
    // rewrite object
    // generate new sum of commit
    // rewrite tree
    // roll back rewrite parant sum
    // 
    // dump it back


}