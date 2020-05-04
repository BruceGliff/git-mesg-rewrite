#pragma once

static char const * EINVALID_ARG = "Invalid arg. Please, use ./git-mesg-rewrite <HEAD~? or hash> <new message>";
static char const * EINVALID_HASH = "Invalid hash.";
static char const * EINVALID_FILE = "Cann't open file.";
static char const * EINVALID_ALLOC = "Cann't allocate memory.";



#define ERROR(msg)                          \
    {                                       \
        fprintf(stderr, "%s\n", msg);       \
        return 0;                           \
    }

#define SHA_LEN 40
#define BLOCK 4096
