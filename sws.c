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

#define TODO printf("TODO line: %d", __LINE__)
#define TEST(x) if(!x) printf("FAILED line: %d\n", __LINE__);
#define BUFFER_SIZE 256

//============================================================================//
// INCLUDES
//============================================================================//
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

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
int ARG_is_port(char* port);
int ARG_is_directory(char* directory);

// Command line
int SHOW_usage();
int SHOW_help();
int SHOW_running();
int SHOW_request(
    char* IP,
    int port,
    char* method,
    char* protocol,
    char* version,
    char* status,
    char* reason,
    char* URI
);

// HTTP parsing
void HTTP_method(char* buffer, char* request);
void HTTP_URI(char* buffer, char* request);
void HTTP_protocol(char* buffer, char*request);
void HTTP_version(char* buffer, char* request);
void HTTP_parse_block(char* buffer, char* request, int block);

// HTTP respond
void HTTP_response(char* buffer, int status, char* reason, char* objects);

// File handling
int FILE_is_file(char* path);
int FILE_is_directory(char* path);
int FILE_in_directory(char* path);

// Server
int SERVER_configure();
void SERVER_listen();

//============================================================================//
// GLOBAL VARIABLES
//============================================================================//

// Configuration
char* CNFG_port;
char CNFG_directory[MAX_PATH + 1];

//============================================================================//
// PROGRAM MAIN
//============================================================================//

int main(int argc, char* argv[]) {

    printf(SWS);

    int i;

    // Parse command line options
    if(argc == 1) {
        return SHOW_usage();
    }

    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            return SHOW_help();
        } else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {

            // TEST URL parsing

            char* request = "GET /dir/test HTTP/1.0";
            char buffer[BUFFER_SIZE];

            printf("REQUEST: %s\n\n", request);
            HTTP_method(buffer, request);
            printf("    METHOD: %s\n", buffer);
            HTTP_URI(buffer, request);
            printf("    URI: %s\n", buffer);
            HTTP_protocol(buffer, request);
            printf("    PROTOCOL: %s\n", buffer);
            HTTP_version(buffer, request);
            printf("    VERSION: %s\n", buffer);
            HTTP_response(buffer, 200, "OK", "<!DOCTYPE HTML><html>Hello World!!!</html>");
            printf("\nRESPONSE:\n\n%s\n", buffer);

            // Test port

            TEST(ARG_is_port("0"));
            TEST(ARG_is_port("65535 "));
            TEST(ARG_is_port(" 25000"));
            TEST(!ARG_is_port("-1"));
            TEST(!ARG_is_port("65536"));
            TEST(!ARG_is_port("Hello World"));

            return EXIT_SUCCESS;
        } else {
            break;
        }
    }

    // Set config values
    if(ARG_is_port(argv[i])) {
        CNFG_port = argv[i];
    } else {
        return EXIT_SUCCESS;
    }
    if(ARG_is_directory(argv[i + 1])) {
        realpath(argv[i + 1], CNFG_directory);
    } else {
        return EXIT_SUCCESS;
    }

    // Start the Server
    if(SERVER_configure()) {
        SERVER_listen();
    };

    return EXIT_SUCCESS;
}

//============================================================================//
// SERVER
//============================================================================//

int SERVER_configure() {
    TODO;
    return 0;
}

void SERVER_listen() {
    TODO;
}

//============================================================================//
// ARGUMENTS
//============================================================================//

int ARG_is_port(char* port) {

    int port_num = (int) strtol(port, (char**) NULL, 10);

    if(strcmp(port, "0") == 0 || port_num != 0) {
        if(port_num >= 0 && port_num <= 65535) {
            return 1;
        }
    }
    printf("The port number you entered: %s is not valid. Please enter an\n"
           "integer between 0 and 65535.\n\n");
    SHOW_usage();
    return 0;
}

int ARG_is_directory(char* directory) {
    return FILE_is_directory(directory);
}

//============================================================================//
// COMMAND LINE
//============================================================================//

int SHOW_usage() {
    printf("Usage:\n  $ ./sws [flags] <port> <directory>\n\n");
    return EXIT_SUCCESS;
}

int SHOW_help() {
    SHOW_usage();
    printf("Option:\n"
           "  -h  --help  Show usage and options\n");
    return EXIT_SUCCESS;
}

int SHOW_running() {
    printf("sws is running on UDP port %d and serving %s\n"
           "press ‘q’ to quit ...", CNFG_port, CNFG_directory);
    return EXIT_SUCCESS;
}

int SHOW_request(
    char* IP,
    int port,
    char* method,
    char* protocol,
    char* version,
    char* status,
    char* reason,
    char* URI
) {
    TODO;
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    // time IP:Port method / protocol/version; HTTP/1.0 status reason; URI
    printf("%s %s:%d %s / %s/%s; HTTP/1.0 %s %s; %s",
        buffer,
        IP,
        port,
        method,
        protocol,
        version,
        status,
        reason,
        URI );
    return 0;
}

//============================================================================//
// HTTP PARSING
//============================================================================//

void HTTP_method(char* buffer, char* request) {
    HTTP_parse_block(buffer, request, 1);
}

void HTTP_URI(char* buffer, char* request) {
    HTTP_parse_block(buffer, request, 2);
}

void HTTP_protocol(char* buffer, char*request) {
    HTTP_parse_block(buffer, request, 3);

    int i;
    for(i = 0; i < strlen(buffer) && i < BUFFER_SIZE; i++) {
        if(buffer[i] == '/') {
            buffer[i] = 0;
        }
    }
}

void HTTP_version(char* buffer, char* request) {
    HTTP_parse_block(buffer, request, 3);

    int i, j;
    int in_version = 0;
    for(i = j = 0; i < strlen(buffer) && i < BUFFER_SIZE; i++) {
        if(in_version) {
            buffer[j++] = buffer[i];
        }
        if(buffer[i] == '/') {
            in_version = 1;
        }
    }
    buffer[j] = 0;
}

void HTTP_parse_block(char* buffer, char* request, int block) {

    // vars
    int i, j;
    int in_block = 0;
    int current_block = 0;

    for(i = j = 0; i < strlen(request) && i < BUFFER_SIZE; i++) {
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

//============================================================================//
// HTTP HANDLING
//============================================================================//

void HTTP_response(char* buffer, int status, char* reason, char* objects) {
    sprintf(buffer, "HTTP/1.0 %d %s\r\n%s", status, reason, objects);
}

//============================================================================//
// FILE HANDLING
//============================================================================//

int FILE_is_file(char* path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int FILE_is_directory(char* path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int FILE_in_directory(char* path) {
    char full_path[MAX_PATH + 1];
    realpath(path, full_path);
    return strcmp(path, full_path, strlen(path)) == 0;
}