#pragma once

#define SHA_LEN 40
#define BLOCK 4096
#define OUT

typedef struct commit_tree
{
    char parent[SHA_LEN+1];
    char current[SHA_LEN+1];
    char new_current[SHA_LEN+1];
    char * data;
    int data_size;
    struct commit_tree * next;
    struct commit_tree * prev;
} commit_tree;

// Return pointer to HEAD commit
commit_tree * get_HEAD();
// Fill fields of node. Return 0 if it is First commit(no parent)
int consturct_commit_node(commit_tree ** node, char const * commit_hash);
// Parse hash to object's path
void construct_path_to_object(char const * commit_hash, OUT char * path_to_objects);
// construct commit-tree
int construct_commit_tree(commit_tree * head, int depth);
