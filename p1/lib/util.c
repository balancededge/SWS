//============================================================================//
// @file show.c
// @author Eric Buss
// @date February 2017
//
// Provides a set of general utility functions.
//
// ===========================================================================//

//============================================================================//
// INCLUDES
//============================================================================//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

//============================================================================//
// FUNCTIONS
//============================================================================//

/**
 * Removes all of the whitespace (as identified by isspace) from a buffer.
 *
 * @param   char*   buffer  write buffer
 * @return  char*           buffer
 */
char* util_no_whitespace(char* buffer) {
    int i, j;
    for(i = j = 0; buffer[j] != 0; i++, j++) {
        while(isspace(buffer[j])) {
            j++;
        }
        buffer[i] = buffer[j];
    }
    buffer[i] = 0;
    return buffer;
}
/**
 * Converts a buffer to uppercase.
 *
 * @param   char*   buffer  write buffer
 * @return  char*           buffer
 */
char* util_toupper(char* buffer) {
    int i;
    for(i = 0; i < strlen(buffer); i++) {
        buffer[i] = toupper((unsigned char) buffer[i]);
    }
    return buffer;
}
/**
 * Zero out a buffer.
 *
 * @param   char*   buffer  write buffer
 * @param   int     n       size to util_zero
 * @return  char*           buffer
 */
char* util_zero(char* buffer, int n) {
    int i;
    for(i = 0; i < n; i++) {
        buffer[i] = 0;
    }
    return buffer;
}
/**
 * Check if a buffer ends with a postfix.
 *
 * @param   char*   source  buffer to compare to
 * @param   char*   postfix end
 * @return  int             1 if postfix matches
 */
int util_endswith(char* source, char* postfix) {
    return strlen(source) > strlen(postfix)
        ? (strcmp((char* ) (source + strlen(source) - strlen(postfix)), postfix) == 0)
        : 0;
}
/**
 * Check if a provided option is set.
 *
 * @param   char*   arg     command line argument
 * @param   char*   flag    option flag
 * @param   char*   verbose option name
 * @return int              1 if the argument is the option
 */
int util_option(char* arg, char* flag, char* verbose) {
    return strcmp(arg, flag) == 0 || strcmp(arg, verbose) == 0;
}