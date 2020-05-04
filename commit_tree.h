#pragma once

#define SHA_LEN 40
#define BLOCK 4096
#define OUT

typedef struct commit_tree
{
    char parent[SHA_LEN+1];
    char current_SHA[SHA_LEN+1];
    char new_SHA[SHA_LEN+1];
    char * data;
    char * encrypted_data;
    int data_size;
    int encrypted_data_size;
    struct commit_tree * next;
    struct commit_tree * prev;
} commit_tree;

// Return pointer to HEAD commit
commit_tree * get_HEAD();
// Fill fields of node. Return 0 if it is First commit(no parent)
int consturct_commit_node(commit_tree ** node, char const * commit_hash);
// construct commit-tree and return pointer to last node
commit_tree * construct_commit_tree(commit_tree * head, int depth);
// construct commit-tree and return pointer to last node
commit_tree * construct_commit_tree_by_hash(commit_tree * head, char const * HASH);
//
int changeChildSHA(commit_tree * current, commit_tree const * parent);
//
int writeData(commit_tree * node);
// Free memory and if flag 1 - delete old files
int destroyTree(commit_tree * node, int delete_flag);