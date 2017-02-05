#ifndef SWS_FILE_H_
#define SWS_FILE_H_

// Exports

char* HTTP_method(char* buffer, const char* request);
char* HTTP_URI(char* buffer, const char* request);
char* HTTP_protocol(char* buffer, const char* request);
char* HTTP_version(char* buffer, const char* request);
char* HTTP_response(
    char* buffer,
    const int status,
    const char* reason,
    const char* objects
);

#endif