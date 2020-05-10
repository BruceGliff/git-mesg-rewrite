#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "message.h"
#include "Error_msg.h"
#include "zlib.h"


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

char * parserMessage(char const * mesg)
{
    char const gpgsig[] = "-----END PGP SIGNATURE-----";
    // +20 for skip '\0'
    char * point_to_gpgsig = strstr(mesg + 20, gpgsig);

    if (point_to_gpgsig)
        point_to_gpgsig += sizeof(gpgsig) + 3; // skip \n\n
    else
        point_to_gpgsig = strstr(mesg + 20, "\n\n") + 2;

    return point_to_gpgsig;
}

char * rewriteMessage(char * old_data, char const * new_message, int * size)
{
    int body_with_msg_size = 0;
    sscanf(old_data, "commit %d", &body_with_msg_size);
    int const old_prefix_size = strlen(old_data);

    char const * message_begin = parserMessage(old_data);

    int  const   message_size = *size - (message_begin - old_data);

    int  const   body_size =  body_with_msg_size - message_size;

    int  const   new_message_size = strlen(new_message);

    int  const   new_body_with_msg_size = body_size + new_message_size;

    char prefix[256];
    memset(prefix, 0, 256);
    int const prefix_size = sprintf(prefix, "commit %d", new_body_with_msg_size);

    int const new_data_size = prefix_size + new_body_with_msg_size + 1;

    char * new_data = (char *) calloc (1, new_data_size);
    memcpy(new_data, prefix, prefix_size);
    memcpy(new_data + prefix_size + 1, old_data + old_prefix_size + 1, body_size);
    memcpy(new_data + prefix_size + 1 + body_size, new_message, new_message_size);

    *size = new_data_size;
    free(old_data);

    return new_data;
}


int getFullHash(char const * HASH, char * fullHASH)
{
    if (strlen(HASH) < 3 || strlen(HASH) > 40)
            ERROR(EINVALID_HASH)
    memset(fullHASH, 0, SHA_LEN+1);

    // find required object
    char const static_path[] = ".git/objects/";
    int const size = sizeof(static_path) - 1;
    char path_to_objects[256];
    memset(path_to_objects, 0, 256);
    memcpy(path_to_objects, static_path, size);
    path_to_objects[size] = '/';
    memcpy(path_to_objects + size + 1, HASH, 2);

    DIR * dirfd = opendir(path_to_objects);
    if (!dirfd)
        ERROR(EINVALID_FILE);

    struct dirent * dir;
    while( (dir = readdir(dirfd)) != NULL)
    {
        // if hit
        if (isSubstr(dir->d_name, HASH + 2))
            break;
    }

    if (!dir)
        ERROR(EINVALID_HASH)

    memcpy(fullHASH, HASH, 2);
    memcpy(fullHASH+2, dir->d_name, 38);

    closedir(dirfd);

    return 1;
}


char * getMessage(char const * path_to_object, int * message_size)
{
    int fd = open(path_to_object, O_RDONLY);
    if (fd < 0)
        ERROR(EINVALID_FILE);
    struct stat file_stat;
    fstat(fd, &file_stat);

    char * decpypted_message = (char *) calloc (file_stat.st_size, 1);
    if (!decpypted_message)
        ERROR(EINVALID_ALLOC);

    int decpypted_message_size = read(fd, decpypted_message, file_stat.st_size);
    close(fd);

    char * message = decompress_data(decpypted_message, decpypted_message_size, message_size);
    free(decpypted_message);
    if (!message)
        ERROR("Cann't get the message");

    return message;
}

int getParentHash(char const * message, char * place_to_write)
{
    char * point_to_parent = strstr(message + 20, "parent");
    if (!point_to_parent)
        return 0;
    
    char static_string[] = "parent";
    memcpy(place_to_write, point_to_parent + sizeof(static_string), SHA_LEN);

    return 1;
}

void construct_path_to_object(char const * commit_hash, char * path_to_objects)
{
    // parse required hash
    char prefix[3];
    prefix[2] = '\0';
    memcpy(prefix, commit_hash, 2);

    // find required object
    memset(path_to_objects, 0, 256);

    char static_path[] = ".git/objects/";
    int const size = sizeof(static_path) - 1;
    memcpy(path_to_objects, static_path, size);
    memcpy(path_to_objects + size, commit_hash, 2);
    path_to_objects[size + 2] = '/';
    memcpy(path_to_objects + size + 3, commit_hash + 2, 38);
}

int change_SHA(char * where, char const * new_SHA)
{
    char const static_parent_str[] = "parent";
    char * toSHA_pointer = strstr(where + 20, static_parent_str);
    if (!toSHA_pointer)
        ERROR("Cann't parse message and fine parent");
    toSHA_pointer += sizeof(static_parent_str);

    memcpy(toSHA_pointer, new_SHA, SHA_LEN);

    return 1;
}   

int get_ref_file(char * HEAD)
{
    int fd = open(".git/HEAD", O_RDONLY);
    if (fd < 0)
        ERROR(EINVALID_FILE);
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
}