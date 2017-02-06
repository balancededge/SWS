#ifndef SWS_SHOW_H_
#define SWS_SHOW_H_

// Exports

int print_title(char* version);
int print_usage();
int print_help();
int print_running(const int port, const char* path);
int print_request(
    const char* IP,
    const int port,
    const char* method,
    const char* protocol,
    const int status,
    const char* reason,
    const char* URI
);

#endif