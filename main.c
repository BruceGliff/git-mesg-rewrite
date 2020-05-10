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

int work_with_tree(commit_tree * head, commit_tree * current, char const * mess);

int main(int argc, char * argv[])
{
    if (argc < 3)
        ERROR(EINVALID_ARG)

int messlen = strlen(argv[2]);
char message[messlen+2];
memcpy(message, argv[2], messlen);
message[messlen] = '\n';
message[messlen+1] = 0;

    // choose propreate algorithm
    if (!strstr(argv[1], "HEAD"))
    {
        char HASH[SHA_LEN+1];
        getFullHash(argv[1], HASH);

        commit_tree * head = get_HEAD();
        commit_tree * current = construct_commit_tree_by_hash(head, HASH);

        work_with_tree(head, current, message);
    }
    else
    {
        int depth = 0;
        sscanf(argv[1], "HEAD~%d", &depth);

        commit_tree * head = get_HEAD();
        commit_tree * current = construct_commit_tree(head, depth);

        work_with_tree(head, current, message);
    }

}


int work_with_tree(commit_tree * head, commit_tree * current, char const * mess)
{

    current->data = rewriteMessage(current->data, mess, &current->data_size);
    current->encrypted_data = compress_data(current->data, current->data_size, &current->encrypted_data_size);
    toSHA1(current->data, current->data_size, current->new_SHA);

    if(strcmp(current->current_SHA, current->new_SHA) == 0)
    {
        destroyTree(head, 0);
        ERROR("Nothing to change. Exit.");
    }
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

    char HEAD_file[256];
    get_ref_file(HEAD_file);
    int fd = open(HEAD_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        perror(0);
        ERROR(EINVALID_FILE);
    }
    write(fd, head->new_SHA, SHA_LEN);
    close(fd);

    destroyTree(head, 1);

    return 0;
}
