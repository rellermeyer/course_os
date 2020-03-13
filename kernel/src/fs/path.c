
#include <fs.h>
#include <path.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <test.h>


const Path root = {
    .length = 1,
    .capacity = 1,
    .array = (uint8_t[]){(uint8_t)'/'},
};

struct DirEntry * path_get_direntry(struct Vfs * vfs, Path * path, enum VfsErr * err) {
    /*
     Algorithm:
     ```
        // find in hashmap

        current: inode = if is_absolute(path) {
            root_inode
        } else {
            process cwd
        }

        for each part in path {
            if part in inode.list_dir() {
                current = inode.child(part)
            } else {
                return NULL
            }
        }

        // store in hashmap

        return curr

     ```
    */
    DirEntry * curr;
    uint8_t * pathpointer;
    uint8_t * endpointer = path->array + path->length;

    if (path_is_absolute(path)) {
        curr = vfs_get_root(vfs);

        // + 1 because we skip the starting '/'
        pathpointer = path->array + 1;

    } else {
        FATAL("Inode lookup only supports absolute path right now.");
        curr = NULL;
        pathpointer = path->array;
    }


    while (true) {
        uint8_t * nextslash = pathpointer;
        for (; *nextslash != '/' && nextslash < endpointer; nextslash++)
            ;

        // FIXME: `curr` may be NULL
        VPArrayList * dirlist = curr->inode->fs_identifier->operations->list_dir(curr, err);
        if (*err != OK) {
            // Error!
            return NULL;
        }

        bool changed = false;
        for (int i = 0; i < dirlist->length; i++) {
            DirEntry * child = vpa_get(dirlist, i);
            // Todo: quick cmp
            if (strncmp(child->name.data, (char *)pathpointer, (nextslash - pathpointer)) == 0) {
                curr = child;
                changed = true;
                break;
            }
        }

        if (!changed) {
            // The path could not be found
            return NULL;
        }

        // Skip the slash
        pathpointer = nextslash + 1;

        if (nextslash == endpointer) { return curr; }
    }
}

Path * path_clone(Path * path) {
    return u8a_clone(path);
}

Path * path_from_string(char * str) {
    U8ArrayList * arr = u8a_create(1);
    u8a_push_string(arr, str);

    return arr;
}

void path_append(Path * path, char * elem) {
    u8a_push(path, '/');
    u8a_push_string(path, elem);
}

void path_parent(Path * path) {
    // for the case of path='.'
    if (path->length == 1 && path->array[0] == '.') { return; }

    // for the case of path=''
    if (path->length == 0) {
        u8a_push(path, (uint8_t)'.');
        return;
    }

    for (isize_t i = path->length; i >= 0; i--) {
        if (path->array[i] == '/') {
            if (i == 0) {
                // If we got to the start, but there's a '/' there, make the path just this '/'
                path->length = 1;
            } else {
                // if we go back and see a /, just make this the new length.
                path->length = i;
            }
            return;
        }
    }

    path->length = 0;
    u8a_push(path, '.');
}

void path_free(Path * path) {
    u8a_free(path);
}

bool path_contents_equal(Path * a, Path * b) {
    if (a->length != b->length) { return false; }

    for (int i = 0; i < a->length; i++) {
        if (a->array[i] != b->array[i]) { return false; }
    }

    return true;
}

void path_print(Path * path) {
    kprintf("Path: (%i) \"", path->length);
    for (int i = 0; i < path->length; i++) { kprintf("%c", path->array[i]); }
    kprintf("\"\n");
}

bool path_is_absolute(Path * path) {
    return path->array[0] == (uint8_t)'/';
}

bool path_is_relative(Path * path) {
    return !path_is_absolute(path);
}

bool path_exists(struct Vfs * vfs, Path * path) {
    DirEntry * pd = path_get_direntry(vfs, path, NULL);
    return pd != NULL;
}

void strip_chars_from_end(Path * p) {
    while (u8a_get(p, p->length - 1) == '.' || u8a_get(p, p->length - 1) == '/') { u8a_pop(p); }
}

void path_filename(Path * p) {
    // partially based on Rust's file_name

    // if path ends on ".." return None
    if (p->length >= 2 && p->array[p->length - 1] == '.' && p->array[p->length - 2] == '.') {
        u8a_resize(p, 0);
        return;
    }

    // strip characters from the back
    while (u8a_get(p, p->length - 1) == '.' || u8a_get(p, p->length - 1) == '/') { p->length--; }

    // Find first '/' from the back
    isize_t filename_start = p->length - 1;
    while (u8a_get(p, filename_start) != '/' && filename_start > -1) { --filename_start; }
    filename_start++;

    size_t filename_length = p->length - filename_start;
    memmove(p->array, p->array + filename_start, filename_length);
    u8a_resize(p, filename_length);
}

Path * path_filename_cloned(Path * path) {
    Path * p = u8a_clone(path);
    path_filename(p);
    return p;
}
