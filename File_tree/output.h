#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct file_t;
struct filter_t;

enum LINES
{
    PIPE,
    SLASH,
    DASH
};

int file_name_cmp(const void *a, const void *b);
int file_size_cmp(const void *a, const void *b);

void print_size(struct file_t *file, struct filter_t *filter);
void print_tree_prefix(char *prefix, struct filter_t *filter);

int print_tree(struct file_t *root, struct filter_t *filter, int layer, char *prefix, bool last);