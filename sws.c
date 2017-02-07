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
#include <arpa/inet.h>
#include <netdb.h>
#include "lib/http.h"
#include "lib/file.h"
#include "lib/show.h"
#include "lib/util.h"

//============================================================================//
// PROTOTYPES
//============================================================================//

int set_port(const char* arg);
int set_socket();
int set_server_address();
int start();
int handle_user();
int handle_request();

//============================================================================//
// VARIABLES
//============================================================================//

int                port;
int                sock;
struct sockaddr_in server_address;
socklen_t          server_sock_length;
struct sockaddr_in client_address;
socklen_t          client_sock_length;
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

    // Configure socket and server_address
    if(!set_socket() || !set_server_address()) {
        return EXIT_FAILURE;
    }

    // Start the Server
    print_running(port, parsed_path);
    start();

    return EXIT_SUCCESS;
}

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

int set_server_address() {
    // Clear space
    memset(&server_address, 0, sizeof server_address);

    // build server_address
    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port        = htons(port);
    server_sock_length             = sizeof(server_address);
    client_sock_length             = sizeof(client_address);

    // Try and bind
    if(bind(sock, (struct sockaddr *) &server_address, server_sock_length) < 0) {
        printf("Failed to bind on %d:%d.", INADDR_ANY, port);
        close(sock);
        return 0;
    }
    return 1;
}

int start() {

    fd_set file_descriptors;

    while(1) {

        // Clear fd
        FD_ZERO(&file_descriptors);
        // Listen to stdin
        FD_SET(STDIN_FILENO, &file_descriptors);
        // Listen to socket
        FD_SET(sock,         &file_descriptors);

        if(select(sock + 1, &file_descriptors, NULL, NULL, NULL) < 0) {
            print_select_error();
            break;
        }
        if(FD_ISSET(STDIN_FILENO, &file_descriptors) && !handle_user()) {
            break;
        }
        if(FD_ISSET(sock, &file_descriptors) && !handle_request()) {
            break;
        }
    }
}

int handle_user() {
    char buffer[MAX_BUFFER];
    fgets(buffer, MAX_BUFFER - 1, stdin);
    util_no_whitespace(buffer);
    LOG("HI");
    if(strcmp(buffer, "q") == 0) {
        printf("Exiting...\n");
        return 0;
    }
    return 1;
}

int handle_request() {

    // To be replaced with recieve from
    int status;
    int send_size;
    ssize_t rec_size;
    char reason  [MAX_BUFFER];
    char method  [MAX_BUFFER];
    char protocol[MAX_BUFFER];
    char uri     [MAX_BUFFER];
    char request [MAX_BUFFER];
    char response[MAX_BUFFER];
    char objects [MAX_BUFFER];

    char* client_IP;
    struct hostent* client_properties;

    // Recieve request
    rec_size = recvfrom(
        sock,
        (void*) request,
        sizeof request,
        0,
        (struct sockaddr*) &client_address,
        &client_sock_length
    );
    if(rec_size < 0) {
        print_recieve_error();
        return 0;
    }
    if(strlen(request) <= 0) {
        return 1;
    }

    // Get Client Info
    client_properties = gethostbyaddr(
        (const char*) &client_address.sin_addr.s_addr,
        sizeof(client_address.sin_addr.s_addr),
        AF_INET
    );
    if(client_properties == NULL) {
        print_client_property_error();
        return 0;
    }
    client_IP = inet_ntoa(client_address.sin_addr);
    if (client_IP == NULL) {
        print_client_resolve_error();
        return 0;
    }

    // Handle BAD REQUEST
    if(
        strcmp(http_method(method, request), "GET") != 0 ||
        strcmp(http_protocol(protocol, request), "HTTP/1.0") != 0
    ) {
        status = 400;
        strcpy(objects, "");

    // Handle NOT FOUND
    } else if(!in_directory(http_URI(uri, request))) {
        status = 404;
        strcpy(objects, "");

    // Handle OK
    } else {
        status = 200;
        read_file(objects, MAX_BUFFER, uri);
    }

    // Build response
    http_response(response, status, http_reason(reason, status), objects);

    // Send response
    send_size = sendto(
        sock,
        response,
        strlen(response),
        0,
        (struct sockaddr *) &client_address,
        client_sock_length
    );
    if(send_size < 0) {
        print_send_error();
        return 0;
    }

    // Log request
    print_request(
        "127.0.0.1",
        port,
        method,
        protocol,
        status,
        reason,
        uri
    );

    return 1;
}