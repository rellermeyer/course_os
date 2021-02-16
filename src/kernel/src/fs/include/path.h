
#ifndef PATH_H
#define PATH_H

#include <stdbool.h>
#include <u8_array_list.h>
#include <vp_array_list.h>

struct Vfs;
enum VfsErr;

typedef U8ArrayList Path;

extern const Path ROOT_PATH;

// Parse the path and return the direntry which corresponds with the path.
struct DirEntry * path_get_direntry(struct Vfs * vfs, Path * path, enum VfsErr * err);

// Duplicates a path.
Path * path_clone(Path * path);

// Creates a new path from a string.
Path * path_from_string(char * str);

// Appends a string to the end of a path.
// Inserts a slash ('/') between the path and the new element.
void path_append(Path * path, char * elem);

// Returns the path without it's last element.
// If the path is absolute and there is only one element in the path,
// or the path is root ('/'), returns '/'.
//
// If the path is relative and there's only one element in the path,
// or the path is the current directory ('.'), returns '.'
//
// If the path is empty, returns root.
//
// Examples:
// '.'      => '.'
// ''       => '/'
// './test' => '.'
// '.test'  => '.'
// 'test'   => '.'
// '/test'  => '/'
// '/a/b/c' => '/a/b'
// './a/b'  => './a'
void path_parent(Path * path);

// Frees a path completely
void path_free(Path * path);

// Returns true if the path starts with '/'
bool path_is_absolute(Path * path);
// Inverse of `path_is_absolute`
bool path_is_relative(Path * path);
// Returns the final component of a path.
void path_filename(Path * path);
// The same as [path_filename] but copies the path.
Path * path_filename_cloned(Path * path);

// Returns the part of the filename before the extension.
// examples:
// ".test"      ==> .test
// "test.txt"   ==> test
// "test"       ==> test
// ""           ==> NULL (no filename)
char * path_filestem(Path * path);

// Returns the part of the filename after the stem.
// examples:
// ".test"      ==> NULL
// "test.txt"   ==> txt
// "test"       ==> NULL
// ""           ==> NULL (no filename)
char * path_extension(Path * path);


// Joins two paths together.
// After a join, the first path (`base`) is extended with the second path
// and the econd path is freed.
void path_join(Path base, Path * path);

// Produces a string representation of the path. The string should be freed.
// The path that is converted into the string is freed by this function.
char * path_into_string(Path * path);

// Test if the paths are exactly equal.
bool path_contents_equal(Path * a, Path * b);

// Tests if the paths point to the same file. (slower)
bool path_parsed_equal(Path * a, Path * b);

// Prints a path.
void path_print(Path * path);

// Checks if a path has an associated DirEntry
bool path_exists(struct Vfs * vfs, Path * path);

#endif
