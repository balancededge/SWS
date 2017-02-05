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

#define TODO        printf("TODO line: %d", __LINE__)
#define MAX_BUFFER 256
#define LOG(_x)     printf("%s\n", _x); fflush(stdout)

//============================================================================//
// INCLUDES
//============================================================================//
#include <sys/types.h>
#include <sys/stat.h>
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

#if !defined(MAX_PATH)
    #define MAX_PATH 4096
#endif

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
int ARG_is_directory(const char* directory);

// Command line
int SHOW_usage();
int SHOW_help();
int SHOW_running();
int SHOW_request(
    const char* IP,
    const int port,
    const char* method,
    const char* protocol,
    const char* version,
    const int status,
    const char* reason,
    const char* URI
);

// HTTP parsing
void HTTP_method(char* buffer, const char* request);
void HTTP_URI(char* buffer, const char* request);
void HTTP_protocol(char* buffer, const char* request);
void HTTP_version(char* buffer, const char* request);
void HTTP_parse_block(char* buffer,const char* request, const int block);

// HTTP respond
void HTTP_response(
    char* buffer,
    const int status,
    const char* reason,
    const char* objects
);

// File handling
int FILE_is_file(const char* path);
int FILE_is_directory(const char* path);
int FILE_in_directory(const char* path);

// Server
int SERVER_configure();
void SERVER_listen();

// Testing
int TEST_test();

//============================================================================//
// GLOBAL VARIABLES
//============================================================================//

// Configuration
int  CNFG_port;
char CNFG_directory[MAX_PATH + 1];
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
        } else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
            return TEST_test();
        } else {
            break;
        }
    }

    // Set config values
    if(!ARG_is_port(argv[i])) {
        return EXIT_SUCCESS;
    }
    if(!ARG_is_directory(argv[i + 1])) {
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
        scanf("%s", buffer);
        LOG(buffer);
        if(strcmp(buffer, "q") == 0) {
            printf("Exiting...");
            break;
        }

        // buffer = readRequest
        char request[MAX_BUFFER];
        char method[MAX_BUFFER];
        char protocol[MAX_BUFFER];
        char version[MAX_BUFFER];
        char URI[MAX_BUFFER];
        char reaon[MAX_BUFFER];

        strcpy(request, buffer);
        HTTP_method(method, request);
        HTTP_protocol(protocol, request);
        HTTP_version(verison, request);
        HTTP_URI(URI, request);

        if(
            strcmp(method,   "GET" ) != 0 ||
            strcmp(protocol, "HTTP") != 0 ||
            strcmp(version,  "1.0" ) != 0
        ) {
            strcpy(reason, "BAD REQUEST");
            HTTP_response(buffer, 400, reason, "");
        } else if(
            FILE_in_directory(HTTP_URI())
        ) {
            strcpy(reason, "NOT FOUND");
            HTTP_response(buffer, 404, reason, "");
        } else {
            srcpy(reason, "OK");

            // Respond with file contents
        }
        // Log request
        SHOW_request(
            "127.0.0.1"
            CNFG_port
            method,
            protocol,
            vertsion,
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
/**
 *
 */
int ARG_is_directory(const char* directory) {
    if(FILE_is_directory(directory)) {
        realpath(directory, CNFG_directory);
        return 1;
    }
    printf("The directory you entered: %s is not valid.", directory);
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
           "press ‘q’ to quit ...", CNFG_port, CNFG_directory);
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
    const char* version,
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
    // time IP:Port method / protocol/version; HTTP/1.0 status reason; URI
    printf("%s %s:%d %s / %s/%s; HTTP/1.0 %d %s; %s",
        buffer,
        IP,
        port,
        method,
        protocol,
        version,
        status,
        reason,
        URI);
    return EXIT_SUCCESS;
}

//============================================================================//
// HTTP PARSING
//============================================================================//
/**
 *
 */
void HTTP_method(char* buffer, const char* request) {
    HTTP_parse_block(buffer, request, 1);
}
/**
 *
 */
void HTTP_URI(char* buffer, const char* request) {
    HTTP_parse_block(buffer, request, 2);

    if(buffer[strlen(buffer) - 1] == '/') {
        strcpy(buffer + strlen(buffer), "index.html");
    }
}
/**
 *
 */
void HTTP_protocol(char* buffer, const char*request) {
    HTTP_parse_block(buffer, request, 3);

    int i;
    for(i = 0; i < strlen(buffer) && i < MAX_BUFFER; i++) {
        if(buffer[i] == '/') {
            buffer[i] = 0;
        }
    }
}
/**
 *
 */
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

//============================================================================//
// HTTP HANDLING
//============================================================================//
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

//============================================================================//
// FILE HANDLING
//============================================================================//
/**
 * Determines whether a given path points to an exsisting file.
 *
 * @param   const char* path    path to file
 * @return  int                 true if file exsists
 */
int FILE_is_file(const char* path) {
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode);
}
/**
 * Determines whether a given path points to an exsisting directory.
 *
 * @param   const char* path    path to directory
 * @return  int                 true if directory exsists
 */
int FILE_is_directory(const char* path) {
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode);
}
/**
 * Determines whether a given path points to a file within the serving
 * directory.
 *
 * @param   const char* path    path to file
 */
int FILE_in_directory(const char* path) {
    char full_path[MAX_PATH + 1];
    realpath(path, full_path);
    return strncmp(
        CNFG_directory,
        full_path,
        strlen(CNFG_directory)
    ) == 0 && FILE_is_file(path);
}

//============================================================================//
// TESTS
//============================================================================//

#define PASS        TEST_results[TEST_index++] = 1; \
                    printf("Passed %s\n", __func__); \
                    return
#define FAIL        TEST_results[TEST_index++] = 0; \
                    printf("Failed %s at line: %d\n", __func__, __LINE__); \
                    return
#define ASSERT(_a)  if(!(_a)) { FAIL; }


int TEST_index = 0;
int TEST_results[MAX_BUFFER];

void test_ARG_is_port() {
    // Valid ports
    ASSERT(ARG_is_port("0"));
    ASSERT(ARG_is_port("65535"));
    ASSERT(ARG_is_port("32000"));
    // Different formatting
    ASSERT(ARG_is_port(" 1"));
    ASSERT(ARG_is_port("65534 "));
    ASSERT(ARG_is_port(" 32001 "));
    // Invalid ports
    ASSERT(!ARG_is_port("-1"));
    ASSERT(!ARG_is_port("65536"));
    ASSERT(!ARG_is_port("Hello World"));
    PASS;
}

void test_ARG_is_directory() {
    // Valid directories
    ASSERT(ARG_is_directory("."));
    ASSERT(ARG_is_directory(".."));
    // Invalid directories
    ASSERT(!ARG_is_directory("sws.c"));
    ASSERT(!ARG_is_directory("Hello World"));
    PASS;
}

void test_HTTP_method() {
    // Parse request
    char* request = "GET /dir/test HTTP/1.0";
    char buffer[MAX_BUFFER];
    HTTP_method(buffer, request);
    // Assert method
    ASSERT(strcmp("GET", buffer) == 0);
    PASS;
}

void test_HTTP_URI() {
    // Parse request
    char* request1 = "GET /dir/test HTTP/1.0";
    char buffer[MAX_BUFFER];
    HTTP_URI(buffer, request1);
    // Assert URI
    ASSERT(strcmp("/dir/test", buffer) == 0);
    // Parse request
    char* request2 = "GET /dir/ HTTP/1.0";
    HTTP_URI(buffer, request2);
    // Assert URI
    ASSERT(strcmp("/dir/index.html", buffer) == 0);
    PASS;
}

void test_HTTP_protocol() {
    // Parse request
    char* request = "GET /dir/test HTTP/1.0";
    char buffer[MAX_BUFFER];
    HTTP_protocol(buffer, request);
    // Assert protocol
    ASSERT(strcmp("HTTP", buffer) == 0);
    PASS;
}

void test_HTTP_version() {
    // Parse request
    char* request = "GET /dir/test HTTP/1.0";
    char buffer[MAX_BUFFER];
    HTTP_version(buffer, request);
    // Assert version
    ASSERT(strcmp("1.0", buffer) == 0);
    PASS;
}

void test_HTTP_response() {
    // Build response
    char buffer[MAX_BUFFER];
    HTTP_response(buffer, 200, "OK", "<!DOCTYPE HTML><html>Hello World!!!</html>");
    // Assert response
    ASSERT(strcmp("HTTP/1.0 200 OK\r\n<!DOCTYPE HTML><html>Hello World!!!</html>", buffer) == 0);
    PASS;
}

void test_FILE_is_file() {
    // Valid files
    ASSERT(FILE_is_file("sws.c"));
    ASSERT(FILE_is_file("README.md"));
    ASSERT(FILE_is_file("./../SWS/sws.c"));
    // Invalid files
    ASSERT(!FILE_is_file("not_a_file.c"));
    ASSERT(!FILE_is_file("."));
    ASSERT(!FILE_is_file("Hello World"));
    PASS;
}

void test_FILE_is_directory() {
    // Valid directories
    ASSERT(FILE_is_directory("."));
    ASSERT(FILE_is_directory(".."));
    ASSERT(FILE_is_directory("test"));
    // Invalid directories
    ASSERT(!FILE_is_directory("sws.c"));
    ASSERT(!FILE_is_directory("Hello World"));
    ASSERT(!FILE_is_directory("test/index.html"))
    PASS;
}

void test_FILE_in_directory() {
    ARG_is_directory(".");
    // Valid files
    ASSERT(FILE_in_directory("sws.c"));
    ASSERT(FILE_in_directory("README.md"));
    ASSERT(FILE_in_directory("./../SWS/sws.c"));
    ASSERT(FILE_in_directory("test/index.html"));
    // Invalid files
    ASSERT(!FILE_in_directory("./not_a_file.c"));
    ASSERT(FILE_in_directory(".."));
    ASSERT(!FILE_in_directory(".."));
    ASSERT(!FILE_in_directory("./Hello World"));
    PASS;
}

int TEST_test() {

    printf("Running Tests\n"
           "=============\n\n");

    // Run tests
    test_ARG_is_port();
    test_ARG_is_directory();
    test_HTTP_method();
    test_HTTP_URI();
    test_HTTP_protocol();
    test_HTTP_version();
    test_HTTP_response();
    test_FILE_is_file();
    test_FILE_is_directory();
    test_FILE_in_directory();

    printf("\nTest Summary\n"
           "============\n");

    // Show summary
    int i;
    for(i = 0; i < TEST_index; i++) {
        printf(TEST_results[i] ? "." : "E");
    }
    printf("\n\n");

    //========================================================================//
    // WORKING CODE SPACE
    //========================================================================//

    SHOW_request(
        "127.0.0.1",
        "3200",
        "GET",
        "HTML",
        "1.0",
        "404",
        "NOT FOUND",
        "/not_a_file");
    return EXIT_SUCCESS;

    //========================================================================//
}