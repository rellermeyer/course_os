
#ifndef FILE_H
#define FILE_H

#include <vp_array_list.h>
#include <path.h>

#include "path.h"

struct FsOperations;

typedef struct File {
    struct DirEntry * dentry;
    size_t file_position;
    const struct FsOperations * operations;
} File;

void create_file(Path * p, enum VfsErr * err);
void create_dir(struct Vfs * vfs, Path * p, enum VfsErr * err);

void remove_dir(Path * p, bool recurse, enum VfsErr * err);
void remove_file(Path * p, enum VfsErr * err);

VPArrayList * list_dir(Path * p, enum VfsErr * err);

File * open_file(Path * p, enum VfsErr * err);
size_t read_file(File * f, uint8_t * buf, size_t length, enum VfsErr * err);
size_t write_file(File * f, uint8_t * buf, size_t length, enum VfsErr * err);
void seek_file(File * f, size_t pos, enum VfsErr * err);
void close_file(File * f, enum VfsErr * err);


#endif
