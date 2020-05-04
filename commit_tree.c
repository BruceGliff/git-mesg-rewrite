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

    char path[256] = "";
    construct_path_to_object(commit_hash, path);
    memcpy((*node)->current_SHA, commit_hash, SHA_LEN);

    (*node)->data = getMessage(path, &(*node)->data_size);
    if (!(*node)->data)
    {
        fprintf(stderr, "or cann't change initial commit.\n");
        exit(1);
    }

    return getParentHash((*node)->data, (*node)->parent);
}

commit_tree * get_HEAD()
{
    char HEAD[256];
    get_ref_file(HEAD);
    int fd = open(HEAD, O_RDONLY);
    if (fd < 0)
        ERROR(EINVALID_FILE);

    char hash[SHA_LEN];
    int size = read(fd, hash, SHA_LEN);
    close(fd);
    if (size != SHA_LEN)
        ERROR(EINVALID_HASH);

    commit_tree * head = NULL;

    int is_first = consturct_commit_node(&head, hash);
    
    return head;
}

commit_tree * construct_commit_tree(commit_tree * head, int depth)
{
    for (int i = 0; i != depth; ++i)
    {   
        if (strlen(head->parent) == 0)
            ERROR("Wrong commit number!");
        consturct_commit_node(&head->prev, head->parent);
        head->prev->next = head;
        head = head->prev;
    }

    return head;
}

int changeChildSHA(commit_tree * current, commit_tree const * parent)
{
    change_SHA(current->data, parent->new_SHA);
    memcpy(current->parent, parent->new_SHA, SHA_LEN);   
}

int writeData(commit_tree * node)
{
    char dirPath[256] = ".git/objects/";
    memcpy(dirPath + 13, node->new_SHA, 2);
    int dir = mkdir(dirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    construct_path_to_object(node->new_SHA, dirPath);

    int fd = open(dirPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        perror(0);
        ERROR(EINVALID_FILE);
    }
    write(fd, node->encrypted_data, node->encrypted_data_size);
    close(fd);
}

int destroyTree(commit_tree * node, int delete_flag)
{
    char path_to_old_object[256];
    commit_tree * intermediate_node = node;
    int res = 0;
    while(intermediate_node)
    {
        construct_path_to_object(intermediate_node->current_SHA, path_to_old_object);

        if (delete_flag)
        {       
            res = remove(path_to_old_object);
            if (res == -1)
                ERROR("Cann't remove old objects");

            char dir_path[256];
            memset(dir_path, 0, 256);
            memcpy(dir_path, path_to_old_object, strlen(path_to_old_object) - 39); 
            res = remove(dir_path);
        }

        node = intermediate_node->prev;

        if (intermediate_node->data)
            free(intermediate_node->data);
        if (intermediate_node->encrypted_data)
            free(intermediate_node->encrypted_data);
        free(intermediate_node);

        intermediate_node = node;
    }
}

commit_tree * construct_commit_tree_by_hash(commit_tree * head, char const * HASH)
{
    while(strcmp(head->current_SHA, HASH) != 0)
    {   
        if (strlen(head->parent) == 0)
            ERROR("Wrong commit number!");
        consturct_commit_node(&head->prev, head->parent);
        head->prev->next = head;
        head = head->prev;
    }

    return head;
}