#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "http.h"

#define MAX_URL 2000

/**
 * Divides a request string into blocks based off whitespace. The requested
 * block is written into the provided buffer.
 *
 * @param   char*       buffer  write buffer
 * @param   const char* request http request to parse
 * @param   const int   block to write
 * @return  char*       buffer
 */
char* http_parse_block(char* buffer, const char* request, const int block) {

    // vars
    int i, j;
    int in_block = 0;
    int current_block = 0;

    for(i = j = 0; i < strlen(request) && i < MAX_URL; i++) {
        if(isspace(request[i])) {
            in_block = 0;
            continue;
        }
        if(!in_block) {
            current_block++;
            in_block = 1;
        }
        if(current_block > block) {
            break;
        }
        if(current_block == block) {
            buffer[j++] = request[i];
        }
    }
    buffer[j] = 0;
    return buffer;
}
/**
 * Returns the http method of a request string.
 *
 * @param   char*       buffer  write buffer
 * @param   const char* request request string
 * @return  char*       buffer
 */
char* http_method(char* buffer, const char* request) {
    return http_parse_block(buffer, request, 1);
}
/**
 * Returns the URI of a request string.
 *
 * @param   char*       buffer  write buffer
 * @param   const char* request request string
 * @return  char*       buffer
 */
char* http_URI(char* buffer, const char* request) {
    http_parse_block(buffer, request, 2);

    if(buffer[strlen(buffer) - 1] == '/') {
        strcpy(buffer + strlen(buffer), "index.html");
    }
    return buffer;
}
/**
 * Returns the http protocol and version blocks of a request string.
 *
 * @param   char*       buffer  write buffer
 * @param   const char* request request string
 * @return  char*       buffer
 */
char* http_protocol(char* buffer, const char*request) {
    return http_parse_block(buffer, request, 3);
}
/**
 * Formats an http response and places it into the provided BUFFER.
 *
 * @param   const int   status  http response status (200, 400, 404)
 * @param   const char* reason  Status reason message
 * @param   const char* objects Any hTML objects to attach to the response
 */
char* http_response(
    const int status,
    const char* reason,
    const char* objects
) {
    char* buffer = (char*) malloc(sizeof(char) * (strlen(objects) + MAX_URL));
    sprintf(buffer, "HTTP/1.0 %d %s\r\n%s\r\n\r\n", status, reason, objects);
    return buffer;
}
/**
 *
 */
 char* http_reason(char* buffer, const int status) {
     switch(status) {
        case 200:
            strcpy(buffer, "OK");
            break;
        case 400:
            strcpy(buffer, "BAD REQUEST");
            break;
        case 404:
            strcpy(buffer, "NOT FOUND");
            break;
     }
     return buffer;
 }