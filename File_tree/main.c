#include "load.h"
#include "output.h"
#include "structs.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void initialize_filter(struct filter_t *filter)
{
    filter->real_size = false;
    filter->sort_by_size = false;
    filter->percentage = false;
    filter->depth = -1;
    filter->lines = ASCII;

    filter->root_size = 0;
    filter->root_blocks = 0;

    filter->error_file = false;
}

int parse_arguments(int argc, char *argv[], struct filter_t *filter)
{
    int arg_count = 1;
    while (arg_count < argc - 1) {
        if (!strcmp(argv[arg_count], "-a") && !filter->real_size) {
            filter->real_size = true;
        } else if (!strcmp(argv[arg_count], "-s") && !filter->sort_by_size) {
            filter->sort_by_size = true;
        } else if (!strcmp(argv[arg_count], "-d") && filter->depth == -1) {
            if (arg_count >= argc - 1 || *argv[arg_count + 1] == '-') {
                fprintf(stderr, "Invalid depth\n");
                return EXIT_FAILURE;
            }
            char *ptr = argv[arg_count + 1];
            while (*ptr != '\0') {
                if (!isdigit(*ptr)) {
                    fprintf(stderr, "Non valid number in depth\n");
                    return EXIT_FAILURE;
                }
                ptr++;
            }
            filter->depth = strtol(argv[arg_count + 1], NULL, 10);
            if (filter->depth < 0) {
                fprintf(stderr, "Negative depth\n");
                return EXIT_FAILURE;
            }
            arg_count++;
        } else if (!strcmp(argv[arg_count], "-p") && !filter->percentage) {
            filter->percentage = true;
        } else if (!strcmp(argv[arg_count], "-u") && filter->lines != UNICODE) {
            filter->lines = UNICODE;
        } else {
            fprintf(stderr, "Invalid argument: %s\n", argv[arg_count]);
            return EXIT_FAILURE;
        }
        arg_count++;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments\n");
        return EXIT_FAILURE;
    }

    struct filter_t arg_filter;
    initialize_filter(&arg_filter);

    struct file_t root;
    initialize_file(&root);

    if (parse_arguments(argc, argv, &arg_filter)) {
        destroy_files(&root);
        return EXIT_FAILURE;
    }

    struct stat statbuf;
    if (stat(argv[argc - 1], &statbuf)) {
        perror("Stat failed");
        destroy_files(&root);
        return EXIT_FAILURE;
    }

    root.type = statbuf.st_mode;
    root.name = calloc(strlen(argv[argc - 1]) + 1, sizeof(char));
    strcpy(root.name, argv[argc - 1]);
    root.size = statbuf.st_size;
    root.block_size = statbuf.st_blksize;
    root.allocated_blocks = statbuf.st_blocks;

    if (S_ISDIR(statbuf.st_mode)) {
        if (load_tree(argv[argc - 1], &root, &arg_filter)) {
            fprintf(stderr, "Loading file tree failed\n");
            destroy_files(&root);
            return EXIT_FAILURE;
        }
    } else if (S_ISREG(statbuf.st_mode)) {
        // do nothing
    } else {
        free(root.name);
        return EXIT_SUCCESS;
    }

    arg_filter.root_size = root.size;
    arg_filter.root_blocks = root.allocated_blocks;

    print_tree(&root, &arg_filter, 0, "", false);
    destroy_files(&root);

    return EXIT_SUCCESS;
}