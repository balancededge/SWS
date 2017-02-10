//============================================================================//
// @file sws.c
// @author Eric Buss
// @date February 2017
// @version 0.0.3
//
// SWS is a ...
//
// Code Resources
// - http://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
// - http://beej.us/guide/bgnet/
// - http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
// - https://connex.csc.uvic.ca/.../udp_server.c
//
// ===========================================================================//

// ===========================================================================//
// MACROS
// ===========================================================================//

#define TODO              printf("TODO line: %d", __LINE__)
#define MAX_BUFFER        2048
#define MAX_RESPONSE_SIZE 1024
#define LOG(_x)           do{ if(debug) { printf("%s\n", _x); fflush(stdout); } }while(0)
#define LOGF(_x, ...)     do{ if(debug) { printf(_x, __VA_ARGS__); fflush(stdout); } }while(0)
#define VERSION           "0.0.3"

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

int                debug = 0;
int                port;
int                sock;
struct sockaddr_in server_address;
socklen_t          server_sock_length;
struct sockaddr_in client_address;
socklen_t          client_sock_length;

//============================================================================//
// FUNCTIONS
//============================================================================//

/**
 * Program entry point.
 */
int main(const int argc, char* argv[]) {

    print_title(VERSION);

    // Parse command line options
    if(argc == 1)
        return print_usage();

    int i;
    for(i = 1; i < argc; i++) {
        if(util_option(argv[i], "-h", "--help"))
            return print_help();
        else if(util_option(argv[i], "-d", "--debug"))
            debug = 1;
        else if(util_option(argv[i], "-v", "--version"))
            return print_version();
        else
            break;
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
/**
 * Validates and sets the port number.
 *
 * @param   const char* arg A string containing the port number
 * @Returns int             1 if the port was succesfully set
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
/**
 * Opens a socket and sets the reuse flag. Will print an error and return 0 if
 * a socket is not available.
 *
 * @return  int     1 if socket is opened
 */
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
/**
 * Reserves the server's address and port. Returns 0 if the address could not
 * bind to the previously created port.
 *
 * @return  int     1 if the address was succesfully bound
 */
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
/**
 * Starts and runs the web server loop.
 */
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
/**
 * Handles user input. Returns 0 if user enters the q key.
 *
 * @return  int     1 if the user entered something other than q
 */
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
/**
 * Handles HTTP request. Returns 0 if any part of the response sequence fails.
 *
 * @return  int     1 if the resposne is succesful
 */
int handle_request() {

    int sent;
    int status;
    int send_size;
    int resp_size;
    ssize_t rec_size;

    // Create and zero buffers
    char buffer  [MAX_RESPONSE_SIZE]; util_zero(buffer,   MAX_RESPONSE_SIZE);
    char reason  [MAX_BUFFER];        util_zero(reason,   MAX_BUFFER);
    char method  [MAX_BUFFER];        util_zero(method,   MAX_BUFFER);
    char protocol[MAX_BUFFER];        util_zero(protocol, MAX_BUFFER);
    char uri     [MAX_BUFFER];        util_zero(uri,      MAX_BUFFER);
    char request [MAX_BUFFER];        util_zero(request,  MAX_BUFFER);

    int client_port;
    char* client_IP;

    // Allocatted strings
    char* response;
    char* objects;
    char* empty = "";

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

    // Get Client IP and port
    client_IP   = inet_ntoa(client_address.sin_addr);
    client_port = ntohs(client_address.sin_port);
    if (client_IP == NULL || client_port < 0) {
        print_client_resolve_error();
        return 0;
    }

    // Handle BAD REQUEST
    if(
        strcmp(http_method(method, request), "GET")          != 0 ||
        strcmp(http_protocol(protocol, request), "HTTP/1.0") != 0 ||
        strncmp(http_URI(uri, request), "/", 1)              != 0 ||
        !util_endswith(request, "\r\n\r\n")
    ) {
        status  = 400;
        objects = empty;

    // Handle NOT FOUND
    } else if(!in_directory(http_URI(uri, request))) {
        status  = 404;
        objects = empty;

    // Handle OK
    } else {
        status = 200;
        objects = read_file(uri);
    }
    // Build response
    response = http_response(status, http_reason(reason, status), objects);

    resp_size = strlen(response);

    // Send response
    for(sent = 0; sent < resp_size; sent += MAX_RESPONSE_SIZE) {

        // Send packet
        send_size = sendto(
            sock,
            (char*) (response + sent),
            strlen((char *) (response + sent)) - 1 > MAX_RESPONSE_SIZE
                ? MAX_RESPONSE_SIZE
                : strlen(response) - 1,
            0,
            (struct sockaddr *) &client_address,
            client_sock_length
        );

        // Check that we succeeded
        if(send_size < 0) {
            print_send_error();
            return 0;
        }
    }
    LOG("REQUEST:");
    LOG(request);
    LOG("RESPONSE:");
    LOG(response);

    // Log request
    print_request(
        client_IP,
        client_port,
        method,
        protocol,
        status,
        reason,
        uri
    );

    return 1;
}