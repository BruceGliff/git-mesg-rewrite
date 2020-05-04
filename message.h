#pragma once

// Return pointer to message begin
char * parserMessage(char const * mesg);
// Return pointer to new messages, delete old one, use size of old mesg, and return size new one
char * rewriteMessage(char * old_message, char const * new_part, int * size);
// Return full hash by the part
int getFullHash(char const * HASH, char * fullHASH);
//  Alloc and return decompressed from path data.\
    Write size of message in message_size
char * getMessage(char const * path_to_object, int * message_size);
// write parent hach to place_to_write[SHA_LEN]. Return 0 if no parent
int getParentHash(char const * message, char * place_to_write);
int isSubstr(char const * where, char const * what);
// Parse hash to object's path. Path must be 256 size
void construct_path_to_object(char const * commit_hash, char * path_to_objects);
// Return pointer to message with replaced SHA
int change_SHA(char * where, char const * new_SHA);
// Write path to HEAD ref
int get_ref_file(char * HEAD);