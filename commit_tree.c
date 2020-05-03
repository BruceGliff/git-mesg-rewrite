#include "commit_tree.h"
#include "Error_msg.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "message.h"


int consturct_commit_node(commit_tree ** node, char const * commit_hash)
{
    *node = (commit_tree *) calloc (1, sizeof(commit_tree));

    char path[256];
    construct_path_to_object(commit_hash, path);
    memcpy((*node)->current, commit_hash, SHA_LEN);

    (*node)->data = getMessage(path, &(*node)->data_size);
    if (!(*node)->data)
    {
        fprintf(stderr, "or cann't change initial commit.\n");
        exit(1);
    }

    return getParentHash((*node)->data, (*node)->parent);
}

void construct_path_to_object(char const * commit_hash, char * path_to_objects)
{
    // parse required hash
    char prefix[3];
    prefix[2] = '\0';
    sscanf(commit_hash, "%2s", prefix);

    // find required object
    memset(path_to_objects, 0, 256);

    sprintf(path_to_objects, ".git/objects/%s/%s", prefix, commit_hash + 2);
}

commit_tree * get_HEAD()
{
    int fd = open(".git/HEAD", O_RDONLY);
    if (fd < 0)
        ERROR(EINVALID_FILE);
    char HEAD[256];
    memset(HEAD, 0, 256);

    int size = read(fd, HEAD, 256);
    close(fd);

    char headHash[256];
    memset(headHash, 0, 256);
    char * dir_beg = strstr(HEAD, "refs");
    if (!dir_beg)
        ERROR("Cann't parse HEAD file");

    memcpy(headHash, dir_beg, size - (dir_beg - HEAD) - 1);

    memset(HEAD, 0, 256);
    sprintf(HEAD, ".git/%s", headHash);
    fd = open(HEAD, O_RDONLY);
    if (fd < 0)
        ERROR(EINVALID_FILE);

    char hash[SHA_LEN];
    size = read(fd, hash, SHA_LEN);
    close(fd);
    if (size != SHA_LEN)
        ERROR(EINVALID_HASH);

    commit_tree * head = NULL;

    int is_first = consturct_commit_node(&head, hash);
    
    return head;
}

int construct_commit_tree(commit_tree * head, int depth)
{
    for (int i = 0; i != depth; ++i)
    {   
        if (strlen(head->parent) == 0)
            ERROR("Wrong commit number!");
        consturct_commit_node(&head->prev, head->parent);
        head->prev->next = head;
        head = head->prev;
    }

    return 1;
}