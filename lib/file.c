#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

char SERVING_PATH[MAX_PATH + 1];

/**
 * Configure the directory that files will be served from.
 *
 * @param   char*  path to directory
 */
int configure_serving_path(const char* path) {
    if(is_directory(path)) {
        realpath(SERVING_PATH, path);
        return 1;
    }
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
    char full_path[MAX_PATH + 1];
    full_path(full_path, path);
    return
        !strncmp(SERVING_PATH, full_path, strlen(SERVING_PATH)) &&
        is_file(full_path);
}
/**
 * Reads the contents of a file into a buffer.
 *
 * @param   char*       buffer  write buffer
 * @param   const int   n       buffer size
 * @param   const char* path    file path
 * @return  int                 success
 */
int read_file(char* buffer, const int n, const char* path) {

    long file_size;
    char full_path[MAX_PATH + 1];
    full_path(full_path, path);

    FILE* file = fopen(full_path, "rb");
    if(file) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        fread(
            buffer,
            1,
            n - 1 > file_size
            ? file_size
            : n - 1,
            file
        );
    } else {
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}