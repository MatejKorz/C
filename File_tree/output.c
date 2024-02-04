#include "output.h"

#include "structs.h"

int file_name_cmp(const void *a, const void *b)
{
    const struct file_t *a_file = *(const struct file_t **) a;
    const struct file_t *b_file = *(const struct file_t **) b;

    const char *a_str = a_file->name;
    const char *b_str = b_file->name;

    for (;; a_str++, b_str++) {
        if (*a_str == '\0' && *b_str != '\0') {
            return -1;
        }
        if (*b_str == '\0' && *a_str != '\0') {
            return 1;
        }
        if (*a_str == '\0' && *b_str == '\0') {
            return strcmp(a_file->name, b_file->name);
        }
        int d = tolower(*a_str) - tolower(*b_str);
        if (d) {
            return d;
        }
    }
}

int file_size_cmp(const void *a, const void *b)
{
    const struct file_t *a_file = *(const struct file_t **) a;
    const struct file_t *b_file = *(const struct file_t **) b;

    if (a_file->size > b_file->size) {
        return -1;
    }
    if (a_file->size < b_file->size) {
        return 1;
    }
    return file_name_cmp(a, b);
}

void print_size(struct file_t *file, struct filter_t *filter)
{
    ssize_t size;
    ssize_t root_size;
    float fsize;
    if (filter->real_size) {
        size = file->size;
        root_size = filter->root_size;
    } else {
        size = file->allocated_blocks * 512;
        root_size = filter->root_blocks * 512;
    }

    ssize_t power_2 = 1024;
    for (int i = 0; i < 6; i++) {
        if (size < power_2 - 1) {
            if (i == 0) {
                fsize = (float) size;
            } else {
                power_2 = power_2 >> 10;
                fsize = ((float) size) / power_2;
            }
            if (filter->percentage) {
                printf("%5.1f", floorf(((size / (float) root_size) * 100) * 10.0f) / 10.0f);
                putchar('%');
                putchar(' ');
            } else {
                printf("%6.1f", floorf(fsize * 10.0f) / 10.0f);
                printf(" %s", SUFFIX[i]);
            }
            return;
        }
        power_2 = power_2 << 10;
    }
}

void print_tree_prefix(char *prefix, struct filter_t *filter)
{
    char *ptr = prefix;
    printf("%.1s", ptr);
    ptr++;
    while (*ptr != '\0') {
        printf("   ");
        printf("%.1s", ptr);
        ptr++;
    }
    printf("%s", filter->lines[DASH]);
}

void print_error_prefix(struct file_t *root, struct filter_t *filter)
{
    if (!filter->error_file) {
        return;
    }
    if (root->error && S_ISDIR(root->type)) {
        printf("? ");
    } else {
        printf("  ");
    }
}

int print_tree(struct file_t *root, struct filter_t *filter, int layer, char *prefix, bool last)
{
    char *curr_prefix = calloc(sizeof(char), strlen(prefix) + 2);
    strcpy(curr_prefix, prefix);
    strcat(curr_prefix, "|");

    if (filter->sort_by_size) {
        qsort(root->children, root->child_count, sizeof(struct file_t *), file_size_cmp);
    } else {
        qsort(root->children, root->child_count, sizeof(struct file_t *), file_name_cmp);
    }

    print_error_prefix(root, filter);
    print_size(root, filter);

    if (layer != 0) {
        char *dir_prefix = malloc(strlen(prefix) + 1);
        strcpy(dir_prefix, prefix);
        if (last) {
            dir_prefix[strlen(prefix) - 1] = '\\';
        }
        print_tree_prefix(dir_prefix, filter);
        free(dir_prefix);
    }
    printf("%s", root->name);
    printf("\n");

    if (layer == filter->depth) {
        free(curr_prefix);
        return EXIT_SUCCESS;
    }

    for (size_t i = 0; i < root->child_count; i++) {
        if (S_ISDIR(root->children[i]->type)) {
            if (i + 1 == root->child_count) {
                curr_prefix[strlen(curr_prefix) - 1] = ' ';
            }
            print_tree(root->children[i], filter, layer + 1, curr_prefix, last || i + 1 == root->child_count);
        } else {
            if (i + 1 == root->child_count) {
                curr_prefix[strlen(curr_prefix) - 1] = '\\';
            }
            print_error_prefix(root->children[i], filter);
            print_size(root->children[i], filter);
            print_tree_prefix(curr_prefix, filter);
            printf("%s", root->children[i]->name);
            printf("\n");
        }
    }
    free(curr_prefix);
    return EXIT_SUCCESS;
}