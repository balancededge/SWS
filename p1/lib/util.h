#ifndef SWS_UTIL_H_
#define SWS_UTIL_H_

// Exports

char* util_no_whitespace(char* buffer);
char* util_toupper(char* buffer);
char* util_zero(char* buffer, int n);
int util_endswith(char* source, char* postfix);
int util_option(char* arg, char* flag, char* verbose);

#endif