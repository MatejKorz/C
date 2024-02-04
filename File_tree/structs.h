#include <stdbool.h>
#include <sys/types.h>

#ifndef STRUCTS_H
#define STRUCTS_H

extern const char *SUFFIX[];
extern const char *ASCII[];
extern const char *UNICODE[];

struct filter_t
{
    bool real_size;
    bool sort_by_size;
    bool percentage;
    int depth;
    const char **lines;

    ssize_t root_size;
    ssize_t root_blocks;

    bool error_file;
};

struct file_t
{
    bool error;

    mode_t type;
    char *name;
    ssize_t size;
    ssize_t block_size;
    ssize_t allocated_blocks;

    size_t child_count;
    size_t capacity;
    struct file_t **children;
};

#endif
