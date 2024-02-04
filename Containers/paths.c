#include "paths.h"

#include "containers.h"
#include "data_source.h"

void initialize_path(struct path_t *path)
{
    path->from_container = 0;
    path->to_container = 0;
    path->distance = 0;
}

bool load_path_uint(unsigned int *path_uint, size_t line_index, const char *function(size_t line_index))
{
    const char *string = function(line_index);
    if (!is_digit(string)) {
        return false;
    }
    unsigned int num = strtoul(string, NULL, 10);
    if (num == UINT_MAX || errno == ERANGE) {
        return false;
    }
    *path_uint = num;
    return true;
}

bool load_path(struct path_t *path, size_t line_index)
{
    initialize_path(path);

    if (!load_path_uint(&(path->from_container), line_index, get_path_a_id)) {
        fprintf(stderr, "Invalid container ID in path file on line %lu\n", line_index + 1);
        return false;
    }
    if (!load_path_uint(&(path->to_container), line_index, get_path_b_id)) {
        fprintf(stderr, "Invalid container ID in path file on line %lu\n", line_index + 1);
        return false;
    }
    if (!load_path_uint(&(path->distance), line_index, get_path_distance)) {
        fprintf(stderr, "Invalid distance in path file on line %lu\n", line_index + 1);
        return false;
    }
    return true;
}

bool path_in_range(size_t line_index)
{
    return get_path_a_id(line_index);
}

size_t number_of_paths()
{
    size_t count = 0;
    while (path_in_range(count)) {
        count++;
    }
    return count;
}

bool load_all_paths(struct path_t *path_array[], size_t array_length)
{
    size_t path_count = 0;
    while (path_count < array_length) {
        struct path_t *new_path = malloc(sizeof(struct path_t));
        if (new_path == NULL) {
            fprintf(stderr, "Path allocation failed\n");
            return false;
        }
        if (!load_path(new_path, path_count)) {
            fprintf(stderr, "Invalid path on line %lu\n", path_count);
            free(new_path);
            return false;
        }
        path_array[path_count] = new_path;
        path_count++;
    }
    path_array[path_count] = NULL;
    return true;
}

int path_compare_from(const void *path1_ptr, const void *path2_ptr)
{
    struct path_t **path1 = (struct path_t **) path1_ptr;
    struct path_t **path2 = (struct path_t **) path2_ptr;

    if ((*path1)->from_container > (*path2)->from_container) {
        return 1;
    }
    if ((*path1)->from_container < (*path2)->from_container) {
        return -1;
    }
    return 0;
}

void delete_all_paths(struct path_t *path_array[])
{
    size_t index = 0;
    while (path_array[index] != NULL) {
        free(path_array[index]);
        index++;
    }
}