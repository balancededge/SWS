#ifndef SWS_HTTP_H_
#define SWS_HTTP_H_

// Exports

char* http_method(char* buffer, const char* request);
char* http_URI(char* buffer, const char* request);
char* http_protocol(char* buffer, const char* request);
char* http_version(char* buffer, const char* request);
char* http_response(
    char* buffer,
    const int status,
    const char* reason,
    const char* objects
);
char* http_reason(char* buffer, const int status);

#endif