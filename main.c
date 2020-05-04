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
#include "zlib.h"

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
        commit_tree * current = construct_commit_tree(head, depth);


        current->data = rewriteMessage(current->data, argv[2], &current->data_size);
        current->encrypted_data = compress_data(current->data, current->data_size, &current->encrypted_data_size);
        toSHA1(current->data, current->data_size, current->new_SHA);
        
        while(current->next)
        {
            current = current->next;
            changeChildSHA(current, current->prev);
            current->encrypted_data = compress_data(current->data, current->data_size, &current->encrypted_data_size);
            toSHA1(current->data, current->data_size, current->new_SHA);
        }
        current = head;
        while (current)
        {
            writeData(current);
            current = current->prev;
        }

        // WRONG DESTINATON
        int fd = open(".git/HEAD", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0)
        {
            perror(0);
            ERROR(EINVALID_FILE);
        }
        write(fd, head->new_SHA, SHA_LEN);
        close(fd);
    }

}
