#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "show.h"

int print_title(char* version) {
    printf("\n"
           "    _____      _____\n"
           "   / __\\ \\ /\\ / / __|\n"
           "   \\__ \\\\ V  V /\\__ \\\n"
           "   |___/ \\_/\\_/ |___/\n"
           "                     @author Eric Buss\n"
           "                     @version %s\n"
           "\n"
           "  A Simple Web Server. Developed as part of the University of Victoria's CSC\n"
           "  361 Computer Networks course.\n\n",
           version
    );
    return EXIT_SUCCESS;
}

int print_usage() {
    printf("Usage:\n  $ ./sws [flags] <port> <directory>\n\n");
    return EXIT_SUCCESS;
}

int print_help() {
    print_usage();
    printf("Option:\n"
           "  -h  --help    Show usage and options\n"
           "  -d  --debug   Show debug messages\n"
           "  -v  --version Show version number\n");
    return EXIT_SUCCESS;
}

int print_version() {
    return EXIT_SUCCESS;
}

int print_running(const int port, const char* path) {
    printf("SWS is running on UDP port %d and serving %s\n"
           "press ‘q’ to quit ...\n", port, path
    );
    return EXIT_SUCCESS;
}

int print_select_error() {
    printf("Error: select returned error code\n");
    return EXIT_SUCCESS;
}

int print_recieve_error() {
    printf("Error: recieve from returned error code\n");
    return EXIT_SUCCESS;
}

int print_client_property_error() {
    printf("Error: cannot obtain client properties\n");
    return EXIT_SUCCESS;
}

int print_client_resolve_error() {
    printf("Error: cannot resolve client IP\n");
    return EXIT_SUCCESS;
}

int print_send_error() {
    printf("Error: cannot send client response\n");
    return EXIT_SUCCESS;
}

int print_request(
    const char* IP,
    const int port,
    const char* method,
    const char* protocol,
    const int status,
    const char* reason,
    const char* URI
) {
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);
    //Sep 12 12:00:00
    strftime(buffer, 26, "%b %d %H:%M:%S", tm_info);
    // time IP:Port method / protocol/version; http/1.0 status reason; URI
    printf("%s %s:%d %s / %s; HTTP/1.0 %d %s; %s\n",
        buffer,
        IP,
        port,
        method,
        protocol,
        status,
        reason,
        URI
    );
    return EXIT_SUCCESS;
}