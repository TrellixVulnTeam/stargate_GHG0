#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_files.h"
#include "files.h"

static void TestWriteChownReadDelete(){
    char path[50] = "./tmp/file_read_write_delete.txt";
    char text[50] = "TestWriteReadDelete()";
    char buffer[50];

    v_write_to_file(path, text);
#if SG_OS == _OS_LINUX
    chown_file(path);
#endif
    get_string_from_file(
        path,
        50,
        buffer
    );
    assert(!strcmp(text, buffer));
    delete_file(path);
    // Test failing to delete
    delete_file(path);
}

static void TestListDir(){
    g_get_dir_list("./tmp");
}

static void TestDirIter(){
    struct DirIter dir_iter;
    char* path = NULL;

    dir_iter_init(&dir_iter, ".");
    while((path = dir_iter_dirs(&dir_iter)) != NULL){
        printf("dir: %s", path);
    }
    dir_iter_close(&dir_iter);

    dir_iter_init(&dir_iter, ".");
    while((path = dir_iter_files(&dir_iter)) != NULL){
        printf("dir: %s", path);
    }
    dir_iter_close(&dir_iter);
}

void TestFilesAll(){
    TestWriteChownReadDelete();
    TestListDir();
    TestDirIter();
}
