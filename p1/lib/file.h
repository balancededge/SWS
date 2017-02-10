#ifndef SWS_FILE_H_
#define SWS_FILE_H_

// Exports
int set_serving_path(const char* path);
int is_file(const char* path);
int is_directory(const char* path);
int in_directory(const char* path);
char* read_file(const char* path);

#endif