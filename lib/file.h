#ifndef SWS_FILE_H_
#define SWS_FILE_H_

// Exports
int set_serving_path(const char* path);
int is_file(const char* path);
int is_directory(const char* path);
int in_directory(const char* path);
int read_file(char* buffer, const int n, const char* path);

#endif