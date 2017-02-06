//============================================================================//
// @file sws.c
// @author Eric Buss
// @date February 2017
// @version 0.0.1
//
// SWS is a ...
//
// Coding Resources
// - http://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
//
// ===========================================================================//

// ===========================================================================//
// DEFINES
// ===========================================================================//

#define TODO       printf("TODO line: %d", __LINE__)
#define MAX_BUFFER 1024
#define LOG(_x)    printf("%s\n", _x); fflush(stdout)

//============================================================================//
// INCLUDES
//============================================================================//
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lib/http.h"
#include "lib/file.h"

const char* SWS = "\n"
"    _____      _____\n"
"   / __\\ \\ /\\ / / __|\n"
"   \\__ \\\\ V  V /\\__ \\\n"
"   |___/ \\_/\\_/ |___/\n"
"                     @author Eric Buss\n"
"                     @version 0.0.1\n"
"\n"
"  A Simple Web Server. Developed as part of the University of Victoria's CSC\n"
"  361 Computer Networks course.\n\n";

//============================================================================//
// PROTOTYPES
//============================================================================//

// Arguments
int ARG_is_port(const char* port);

// Command line
int SHOW_usage();
int SHOW_help();
int SHOW_running();
int SHOW_request(
    const char* IP,
    const int port,
    const char* method,
    const char* protocol,
    const int status,
    const char* reason,
    const char* URI
);

// Server
int SERVER_configure();
void SERVER_listen();

//============================================================================//
// GLOBAL VARIABLES
//============================================================================//

// Configuration
int  CNFG_port;
int  CNFG_sock;
struct sockaddr_in CNFG_serveradd;
ssize_t CNFG_recsize;
socklen_t CNFG_fromlen;

//============================================================================//
// PROGRAM MAIN
//============================================================================//
/**
 * Program entry point.
 */
int main(const int argc, char* argv[]) {
    printf("%s", SWS);

    int i;

    // Parse command line options
    if(argc == 1) {
        return SHOW_usage();
    }

    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            return SHOW_help();
        } else {
            break;
        }
    }
    char* port = argv[i];
    char* path = argv[i + 1];

    // Set config values
    if(!ARG_is_port(port)) {
        return EXIT_SUCCESS;
    }
    if(!set_serving_path(path)) {
        printf("The directory you entered: %s cannot be served from.", path);
        return EXIT_SUCCESS;
    }

    // Start the Server
    LOG("Starting server configuration");
    if(SERVER_configure()) {
        LOG("Starting server listening");
        SERVER_listen();
    };

    return EXIT_SUCCESS;
}

//============================================================================//
// SERVER
//============================================================================//
/**
 *
 */
int SERVER_configure() {

    // Create socket
    CNFG_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // Check that we succeeded
    if(CNFG_sock < 0) {
        printf("Failed to open socket. Exiting...");
        return 0;
    }
    // Let go of socket on exit/crash
    int option = 1;
    setsockopt(
        CNFG_sock,
        SOL_SOCKET,
        SO_REUSEADDR,
        (const void *)&option ,
        sizeof(int)
    );
    // Clear garbage
    memset(&CNFG_serveradd, 0, sizeof CNFG_serveradd);

    // build address
    CNFG_serveradd.sin_family      = AF_INET;
    CNFG_serveradd.sin_addr.s_addr = htonl(INADDR_ANY);
    CNFG_serveradd.sin_port        = htons(CNFG_port);
    CNFG_fromlen                   = sizeof(CNFG_serveradd);

    // Try and bind
    if (bind(
        CNFG_sock,
        (struct sockaddr *)&CNFG_serveradd,
        sizeof CNFG_serveradd
    ) < 0) {
        printf("Failed to bind on %d:%d.", INADDR_ANY, CNFG_port);
        close(CNFG_sock);
        return 0;
    }
    return 1;
}
/**
 *
 */
void SERVER_listen() {

    char buffer[MAX_BUFFER];
    int select_result;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    LOG("Entering main loop");
    while(1) {

        LOG("Before select");

        select_result = select(1, &read_fds, NULL, NULL, NULL);

        LOG("After select");

        fflush(stdout);
        fgets(buffer, MAX_BUFFER - 1, stdin);
        LOG(buffer);
        if(strcmp(buffer, "q\n") == 0) {
            printf("Exiting...\n");
            break;
        }

        // buffer = readRequest
        int  status;
        char request[MAX_BUFFER];
        char method[MAX_BUFFER];
        char protocol[MAX_BUFFER];
        char URI[MAX_BUFFER];
        char reason[MAX_BUFFER];
        char contents[MAX_BUFFER];

        strcpy(request, buffer);
        http_method(method, request);
        http_protocol(protocol, request);
        http_URI(URI, request);

        printf("method: %s -> %d\n", method,   strcmp(method,   "GET"));
        printf("method: %s -> %d\n", protocol, strcmp(protocol, "http/1.0"));

        if(
            strcmp(method,   "GET"     ) != 0 ||
            strcmp(protocol, "http/1.0") != 0
        ) {
            status = 400;
            strcpy(reason, "BAD REQUEST");
            http_response(buffer, status, reason, "");
        } else if(
            !in_directory(URI) || !read_file(contents, MAX_BUFFER, URI)
        ) {
            status = 404;
            strcpy(reason, "NOT FOUND");
            http_response(buffer, status, reason, "");
        } else {
            status = 200;
            strcpy(reason, "OK");
            printf("\n%s\n", contents);

            // Respond with file contents
        }
        // Log request
        SHOW_request(
            "127.0.0.1",
            CNFG_port,
            method,
            protocol,
            status,
            reason,
            URI
        );
    }
    close(CNFG_sock);
    return;
}

//============================================================================//
// ARGUMENTS
//============================================================================//
/**
 * @param const char*   port    port number
 * @returns
 */
int ARG_is_port(const char* port) {

    const int port_num = (int) strtol(port, (char**) NULL, 10);

    if(strcmp(port, "0") == 0 || port_num != 0) {
        if(port_num >= 0 && port_num <= 65535) {
            CNFG_port = port_num;
            return 1;
        }
    }
    printf("The port number you entered: %s is not valid. Please enter an\n"
           "integer between 0 and 65535.\n\n", port);
    SHOW_usage();
    return 0;
}

//============================================================================//
// COMMAND LINE
//============================================================================//
/**
 *
 */
int SHOW_usage() {
    printf("Usage:\n  $ ./sws [flags] <port> <directory>\n\n");
    return EXIT_SUCCESS;
}
/**
 *
 */
int SHOW_help() {
    SHOW_usage();
    printf("Option:\n"
           "  -h  --help  Show usage and options\n"
           "  -t  --test  Run tests\n");
    return EXIT_SUCCESS;
}
/**
 *
 */
int SHOW_running() {
    printf("sws is running on UDP port %d and serving %s\n"
    "press ‘q’ to quit ...", CNFG_port, get_serving_path());
    return EXIT_SUCCESS;
}
/**
 *
 */
int SHOW_request(
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
    printf("%s %s:%d %s / %s; http/1.0 %d %s; %s\n",
        buffer,
        IP,
        port,
        method,
        protocol,
        status,
        reason,
        URI);
    return EXIT_SUCCESS;
}