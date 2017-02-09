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

char* util_toupper(char* buffer) {
    int i;
    for(i = 0; i < strlen(buffer); i++) {
        buffer[i] = toupper((unsigned char) buffer[i]);
    }
    return buffer;
}

char* util_zero(char* buffer, int n) {
    int i;
    for(i = 0; i < n; i++) {
        buffer[i] = 0;
    }
    return buffer;
}

int util_endswith(char* source, char* postfix) {
    return strlen(source) > strlen(postfix)
        ? (strcmp((char* ) (source + strlen(source) - strlen(postfix)), postfix) == 0)
        : 0;
}