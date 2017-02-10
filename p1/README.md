# SWS

A Simple Web Server.

Auther Eric Buss V00818492 CSC 361-BO3

## Compilation and Usage

SWS is compiled and run from the command line. To compile SWS type
```bash
$ make
```
in the root project directory. To run SWS type
```
./sws <port> <directory>
```

## Design

SWS source is distrubted among the following files
```
SWS
|--sws.c
+--lib
   |--file.c    // Provides file utilies
   |--file.h
   |--http.c    // Provides HTTP parsing functions
   |--http.h
   |--show.c    // Provides output messages
   |--show.h
   |--util.c    // Provides miscellanius utility functions
   |--util.h
```
`sws.c` contains the base server code. The `lib` files provide various utility
functions. The main program loop undergoes the following process

 1. Command line arguments are parsed
 2. Socket is opened
 3. Socket is bound
 4. Wait on input (user or socket)
 5. Return to step 4

The server sticks to a number of design decision. First is to use primitive
types where possible. Second is to avoid dynamic memory (`malloc` is used only
twice, in `file.c`'s `read_file` function and `http.c`'s `http_response`
function). This keeps all of the source relatively simple and avoids seg faults.

## Support

SWS is meant for unix machines only. This project is not
recieving furhter development.

## Code Resources

*Beej's network guide in particular was invaluable to this project.

 - http://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
 - http://beej.us/guide/bgnet/
 - http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
 - https://connex.csc.uvic.ca/.../udp_server.c


