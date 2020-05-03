#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


#include "Error_msg.h"
#include "SHAimpl/sha1.h"
#include "commit_tree.h"
#include "message.h"

#define OUT

int main(int argc, char * argv[])
{
    if (argc < 3)
        ERROR(EINVALID_ARG)

    // choose propreate algorithm
    if (!strstr(argv[1], "HEAD"))
    {
        /*
        // Get object file
        char editableMSG[BLOCK];
        memset(editableMSG, 0, BLOCK);
        char path_to_object[256];
        FILE * object = getObject(argv[1], path_to_object, 0);
        if (!object)
            ERROR(EINVALID_FILE);

        int size = decompress_file(object, editableMSG);
        if (!size)
            ERROR("Cann't decompress");

        fclose(object); 

        // Parse msg
        char * msgBegin = parserMessage(editableMSG);

        // Write New msg
        int newSize = rewriteMessage(editableMSG, msgBegin, argv[2], size);

        // Calc new hsum
        char sha[40];
        toSHA1(editableMSG, newSize, sha);
        
        // Composemsg
        FILE * newObject = fopen(path_to_object, "wb");
        if (!newObject)
            ERROR(EINVALID_FILE);

        compress_data(editableMSG, newSize, newObject);

        fclose(newObject);*/
    }
    else
    {
        int depth = 0;
        sscanf(argv[1], "HEAD~%d", &depth);

        commit_tree * head = get_HEAD();
        construct_commit_tree(head, depth);

        commit_tree * required = NULL;
        while (head)
        {
            required = head;
            head = head->prev;
        }
        required->data = rewriteMessage(required->data, argv[2], &required->data_size);

        toSHA1(required->data, required->data_size, required->new_current);

    }

}
