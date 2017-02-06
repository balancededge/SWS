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
#define MAX_BUFFER 2000
#define LOG(_x)    printf("%s\n", _x); fflush(stdout)
#define VERSION    "0.0.2"

//============================================================================//
// INCLUDES
//============================================================================//
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lib/http.h"
#include "lib/file.h"
#include "lib/show.h"
#include "lib/util.h"

//============================================================================//
// PROTOTYPES
//============================================================================//

// Arguments
int ARG_is_port(const char* port);

// Server
int SERVER_configure();
void SERVER_listen();

//============================================================================//
// GLOBAL VARIABLES
//============================================================================//

// Configuration
int                port;
int                sock;
struct sockaddr_in address;
ssize_t            recsize;
socklen_t          sock_length;

//============================================================================//
// PROGRAM MAIN
//============================================================================//
/**
 * Program entry point.
 */
int main(const int argc, char* argv[]) {

    print_title(VERSION);

    int i;

    // Parse command line options
    if(argc == 1) {
        return print_usage();
    }
    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            return print_help();;
        } else {
            break;
        }
    }
    char* parsed_port = argv[i];
    char* parsed_path = argv[i + 1];

    // Configure port and serving path
    if(!set_port(parsed_port) || !set_serving_path(parsed_path)) {
        print_usage();
        return EXIT_FAILURE;
    }

    // Configure socket and address
    if(!set_socket() || !set_address()) {
        return EXIT_FAILURE;
    }

    // Start the Server
    print_running(port, parsed_path);
    start();

    return EXIT_SUCCESS;
}
/**
 *
 *
void SERVER_listen() {

    char buffer[MAX_BUFFER];
    int select_result;


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
        printf("method: %s -> %d\n", protocol, strcmp(protocol, "GTTP/1.0"));

        if(
            strcmp(method,   "GET"     ) != 0 ||
            strcmp(protocol, "HTTP/1.0") != 0
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
        print_request(
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
*/

int set_port(const char* arg) {
    port = (int) strtol(arg, (char**) NULL, 10);
    if(strcmp(arg, "0") == 0 || port != 0) {
        if(port >= 0 && port <= 65535) {
            return 1;
        }
    }
    printf("The port number you entered: %d is not valid. Please enter an\n"
           "integer between 0 and 65535.\n\n", port
    );
    return 0;
}

int set_socket() {
    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Check that we succeeded
    if(sock < 0) {
        printf("Failed to open socket. Exiting...");
        return 0;
    }

    // Let go of socket on exit/crash
    int option = 1;
    setsockopt(
        sock,
        SOL_SOCKET,
        SO_REUSEADDR,
        (const void *)&option ,
        sizeof(int)
    );
    return  1;
}

int set_address() {
    // Clear space
    memset(&address, 0, sizeof address);

    // build address
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port        = htons(port);
    sock_length             = sizeof(address);

    // Try and bind
    if (bind(sock, (struct sockaddr *)&address, sizeof address) < 0) {
        printf("Failed to bind on %d:%d.", INADDR_ANY, port);
        close(sock);
        return 0;
    }
    return 1;
}

int start() {

    fd_set read_fds;
    // Clear fd
    FD_ZERO(&read_fds);
    // Listen to stdin
    FD_SET(STDIN_FILENO, &read_fds);
    // Listen to socket
    FD_SET(sock,         &read_fds);

    while(1) {
        if(select(1, &read_fds, NULL, NULL, NULL) < 0) {
            print_select_error();
            break;
        }
        if(!handle_user() || !handle_request()) {
            break;
        }
    }
}

int handle_user() {
    char buffer[MAX_BUFFER];
    fgets(buffer, MAX_BUFFER - 1, stdin);
    util_no_whitespace(buffer);

    if(strcmp(buffer, "q") == 0) {
        printf("Exiting...\n");
        return 0;
    }
    return 1;
}

int handle_request() {
    return 1;
}