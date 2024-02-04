#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct file_t;
struct filter_t;

void initialize_file(struct file_t *file);
int add_file(struct file_t *root, struct stat *statbuf, char *name);
int load_tree(char *path, struct file_t *root, struct filter_t *filter);
void destroy_files(struct file_t *root);