#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

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

int util_endswith(char* source, char* postfix) {
    return strlen(source) > strlen(postfix)
        ? (strcmp(source + strlen(source) - strlen(postfix), postfix) == 0)
        : 0;
}