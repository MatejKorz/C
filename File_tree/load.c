#include "load.h"

#include "structs.h"

void initialize_file(struct file_t *file)
{
    file->error = false;

    file->type = 0;
    file->name = NULL;
    file->size = -1;
    file->block_size = -1;
    file->allocated_blocks = -1;

    file->child_count = 0;
    file->capacity = 10;
    file->children = calloc(file->capacity, sizeof(struct file_t *));
}

int add_file(struct file_t *root, struct stat *statbuf, char *name)
{
    if (root->child_count >= root->capacity) {
        root->capacity = 2 * root->capacity;
        struct file_t **tmp;
        if ((tmp = realloc(root->children, (sizeof(struct file_t *) * root->capacity))) == NULL) {
            // TODO free
            fprintf(stderr, "Reallocation failed\n");
            return EXIT_FAILURE;
        }
        root->children = tmp;
    }

    struct file_t *new_file;
    if ((new_file = malloc(sizeof(struct file_t))) == NULL) {
        // TODO free
        fprintf(stderr, "File allocation failed\n");
        return EXIT_FAILURE;
    }
    initialize_file(new_file);

    new_file->type = statbuf->st_mode;
    new_file->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(new_file->name, name);
    new_file->size = statbuf->st_size;
    new_file->block_size = statbuf->st_blksize;
    new_file->allocated_blocks = statbuf->st_blocks;

    root->children[root->child_count] = new_file;
    root->child_count += 1;
    return EXIT_SUCCESS;
}

int load_tree(char *path, struct file_t *root, struct filter_t *filter)
{
    DIR *dir = NULL;
    struct stat statbuf;
    if ((dir = opendir(path)) != NULL) {
        struct dirent *dir_entry = NULL;
        while ((dir_entry = readdir(dir)) != NULL) {
            if (!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, "..")) {
                continue;
            }
            char filepath[2048] = { 0 };
            if (filepath[strlen(filepath)] == '/') {
                snprintf(filepath, sizeof(filepath), "%s%s", path, dir_entry->d_name);
            } else {
                snprintf(filepath, sizeof(filepath), "%s/%s", path, dir_entry->d_name);
            }
            if (stat(filepath, &statbuf)) {
                perror("Stat failed");
            }
            if (S_ISREG(statbuf.st_mode)) {
                if (add_file(root, &statbuf, dir_entry->d_name)) {
                    // TODO free
                    closedir(dir);
                    return EXIT_FAILURE;
                }
                root->size += statbuf.st_size;
                root->allocated_blocks += statbuf.st_blocks;
            } else if (S_ISDIR(statbuf.st_mode)) {
                if (add_file(root, &statbuf, dir_entry->d_name)) {
                    // TODO free
                    closedir(dir);
                    return EXIT_FAILURE;
                }
                if (load_tree(filepath, root->children[root->child_count - 1], filter)) {
                    // TODO free
                    closedir(dir);
                    return EXIT_FAILURE;
                }
                root->size += root->children[root->child_count - 1]->size;
                root->allocated_blocks += root->children[root->child_count - 1]->allocated_blocks;
                root->error = root->children[root->child_count - 1]->error;
            }
        }
        closedir(dir);
    } else {
        perror("Opendir failed");
        root->error = true;
        filter->error_file = true;
    }
    return EXIT_SUCCESS;
}

void destroy_files(struct file_t *root)
{
    for (size_t i = 0; i < root->child_count; i++) {
        destroy_files(root->children[i]);
        free(root->children[i]);
    }
    free(root->children);
    free(root->name);
}
