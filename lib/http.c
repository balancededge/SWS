#include "http.h";

#define MAX_BUFFER 1024

void HTTP_method(char* buffer, const char* request) {
    HTTP_parse_block(buffer, request, 1);
}

void HTTP_URI(char* buffer, const char* request) {
    HTTP_parse_block(buffer, request, 2);

    if(buffer[strlen(buffer) - 1] == '/') {
        strcpy(buffer + strlen(buffer), "index.html");
    }
}

void HTTP_protocol(char* buffer, const char*request) {
    HTTP_parse_block(buffer, request, 3);

    int i;
    for(i = 0; i < strlen(buffer) && i < MAX_BUFFER; i++) {
        if(buffer[i] == '/') {
            buffer[i] = 0;
        }
    }
}

void HTTP_version(char* buffer, const char* request) {
    HTTP_parse_block(buffer, request, 3);

    int i, j;
    int in_version = 0;
    for(i = j = 0; i < strlen(buffer) && i < MAX_BUFFER; i++) {
        if(in_version) {
            buffer[j++] = buffer[i];
        }
        if(buffer[i] == '/') {
            in_version = 1;
        }
    }
    buffer[j] = 0;
}
/**
 * Divides a request string into blocks based off whitespace. The requested
 * block is written into the provided buffer.
 *
 * @param   char*       buffer  write buffer
 * @param   const char* request HTTP request to parse
 * @param   const int   block to write
 */
void HTTP_parse_block(char* buffer, const char* request, const int block) {

    // vars
    int i, j;
    int in_block = 0;
    int current_block = 0;

    for(i = j = 0; i < strlen(request) && i < MAX_BUFFER; i++) {
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
}
/**
 * Formats an HTTP response and places it into the provided BUFFER.
 *
 * @param   char*       buffer  write buffer
 * @param   const int   status  HTTP response status (200, 400, 404)
 * @param   const char* reason  Status reason message
 * @param   const char* objects Any hTML objects to attach to the response
 */
void HTTP_response(
    char* buffer,
    const int status,
    const char* reason,
    const char* objects
) {
    sprintf(buffer, "HTTP/1.0 %d %s\r\n%s", status, reason, objects);
}