#ifndef UTIL_FILES_H
#define UTIL_FILES_H

#include <dirent.h>
#include <sys/stat.h>
/*Standard string sizes*/
#define XLARGE_STRING 1048576
#define LARGE_STRING  65536 //1048576
#define MEDIUM_STRING  32768 //262144 //8192
#define SMALL_STRING  16384 //65536 //512
#define TINY_STRING 4096 //16384 //32

#define TERMINATING_CHAR '\\'

typedef struct st_dir_list {
    char** dir_list;
    int dir_count;
}t_dir_list;

/* Example usage:

    struct DirIter dir_iter;
    dir_iter_init(&dir_iter, "/some/path");
    char* path;
    while((path = dir_iter_dirs(&dir_iter)) != NULL){
        // do something with path
    }
	dir_iter_close(&dir_iter);

*/
struct DirIter {
    DIR* dir;
    char path[1024];
    char tmp[1024];
};

void dir_iter_init(struct DirIter*, char*);
void dir_iter_close(struct DirIter*);
char* dir_iter_dirs(struct DirIter*);
char* dir_iter_files(struct DirIter*);

int is_dir(char*);
int is_file(char*);

void chown_file(const char *file_name);
int i_file_exists(char*);
void get_string_from_file(const char* a_file, int a_size, char* a_buf);

void v_write_to_file(char * a_file, char * a_string);
void v_append_to_file(char * a_file, char * a_string);
int i_file_exists(char * f_file_name);
t_dir_list* g_get_dir_list(char * a_dir);
void delete_file(char* path);

// Retrieve a config setting from ~/stargate/config/
void get_file_setting(
    char * a_dest,
    char * a_name,
    char * a_default
);

#endif
