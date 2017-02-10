//============================================================================//
// @file file.c
// @author Eric Buss
// @date February 2017
//
// Provides a set of functions for working with files.
//
// ===========================================================================//

//============================================================================//
// INCLUDES
//============================================================================//

#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

// ===========================================================================//
// MACROS
// ===========================================================================//

// *soft* PATH limit
// http://insanecoding.blogspot.ca/2007/11/pathmax-simply-isnt.html
#ifndef MAX_PATH
    #define MAX_PATH 4096
#endif

//============================================================================//
// VARIABLES
//============================================================================//

char* file_buffer = NULL;
char SERVING_PATH[MAX_PATH + 1];

//============================================================================//
// FUNCTIONS
//============================================================================//

/**
 * Configure the directory that files will be served from.
 *
 * @param   char*  path to directory
 * @return  int    1 if path was a directory
 */
int set_serving_path(const char* path) {
    if(is_directory(path)) {
        realpath(path, SERVING_PATH);
        return 1;
    }
    printf("The directory you entered: %s cannot be served from.\n", path);
    return 0;
}
/**
 * Determines the full path of a file appended to the configured directory.
 *
 * @param   char*       buffer  write buffer
 * @param   const char* path    relative/unprefixed path
 * @return  char*               buffer
 */
char* full_path(char* buffer, const char* path) {
    char tmp[MAX_PATH + 1];
    sprintf(tmp, "%s%s", SERVING_PATH, path);
    realpath(tmp, buffer);
    return buffer;
}
/**
 * Determines whether a given path points to an exsisting file.
 *
 * @param   const char* path    path to file
 * @return  int                 true if file exsists
 */
int is_file(const char* path) {
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode);
}
/**
 * Determines whether a given path points to an exsisting directory.
 *
 * @param   const char* path    path to directory
 * @return  int                 true if directory exsists
 */
int is_directory(const char* path) {
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode);
}
/**
 * Determines whether a given path points to a file within the serving
 * directory.
 *
 * @param   const char* path    path to file
 * @return  int                 if path is a file within the serving directory
 */
int in_directory(const char* path) {
    char full[MAX_PATH + 1];
    full_path(full, path);
    return
        strncmp(SERVING_PATH, full, strlen(SERVING_PATH)) == 0 &&
        is_file(full);
}
/**
 * Reads the contents of a file into an allocated buffer. The buffer is freed
 * and recreated every time the function is called.
 *
 * @param   const int   n       buffer size
 * @param   const char* path    file path
 * @return  int                 success
 */
char* read_file(const char* path) {

    long file_size;
    char full[MAX_PATH + 1];
    full_path(full, path);

    // Free the old buffer if it exsists
    if(file_buffer != NULL) {
        free(file_buffer);
    }

    FILE* file = fopen(full, "rb");
    if(file) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        file_buffer = (char*) malloc(sizeof(char) * (file_size + 1));
        fseek(file, 0, SEEK_SET);
        file_buffer[0] = 0;
        fread(file_buffer, 1, file_size, file);
        file_buffer[file_size] = 0;
    } else {
        fclose(file);
        return file_buffer;
    }
    fclose(file);
    return file_buffer;
}