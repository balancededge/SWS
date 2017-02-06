#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

char* util_no_whitespace(char* buffer) {
    int i, j;
    for(i = j = 0; buffer[j] != 0; j) {
        buffer[i] = buffer[j++];
        while(isspace(buffer[j])) {
            j++;
        }
    }
    return buffer;
}