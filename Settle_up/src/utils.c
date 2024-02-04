#include "utils.h"

#include "errors.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void object_destroy(void *o)
{
    struct object *object = (struct object *) o;

    if (object->destruct != NULL) {
        object->destruct(o);
    }
}

void object_set_destructor(void *o, void (*destructor)(void *))
{
    struct object *object = (struct object *) o;

    object->destruct = destructor;
    object->initialized = 1;
}

void object_avoid_duplicit_initialization(void *o)
{
    struct object *object = (struct object *) o;

    OP(!object->initialized, DUPLICIT_INITIALIZATION);
}

char *copy_string(const char *str)
{
    char *copy;
    OP(copy = (char *) malloc(strlen(str) + 1), ALLOCATION_FAILED);
    strcpy(copy, str);
    return copy;
}

char *trim_string(char *begin, char **end)
{
    while (*begin && isspace(*begin))
        ++begin;
    if (end) {
        char *last_char = *end = begin;
        while (**end) {
            if (!isspace(**end))
                last_char = *end;
            ++(*end);
        }
        *end = last_char;
        if (**end && !isspace(**end))
            ++(*end);
    }
    return begin;
}

int empty_string(char *str)
{
    char *end;
    str = trim_string(str, &end);
    return str == end;
}

char *words_end(char *string)
{
    return get_token(string, isspace);
}

char *get_token(char *string, int (*condition)(int))
{
    while (*string && !condition(*string))
        ++string;
    return string;
}

int char_count(const char *string, char letter)
{
    int result = 0;
    for (; *string; ++string) {
        if (*string == letter)
            ++result;
    }
    return result;
}

char *read_line(FILE *input)
{
    int capacity = 16;
    int size = 0;
    char *buffer = (char *) malloc(capacity + 1);
    int c;

    while ((c = fgetc(input)) != EOF && c != '\n') {
        if (size == capacity) {
            capacity *= 2;
            char *tmp = (char *) realloc(buffer, capacity + 1);
            if (!tmp) {
                free(buffer);
                error_happened(ALLOCATION_FAILED);
            }
            buffer = tmp;
        }
        buffer[size++] = c;
    }
    buffer[size] = '\0';
    return buffer;
}

int is_decimal(char *string)
{
    char *ptr = string;
    while (*ptr != '\0') {
        if (!isdigit(*ptr) && *ptr != '.') {
            return 1;
        }
        ptr++;
    }
    return 0;
}

int correct_num_format(char *string)
{
    char *ptr = string;
    bool dot = false;
    while (*ptr != '\0') {
        if (!isdigit(*ptr) && *ptr != '.') {
            return 1;
        }
        if (*ptr == '.') {
            if (dot) {
                return 1;
            }
            dot = true;
        }
        ptr++;
    }
    return 0;
}

int power_of_ten(int exponent)
{
    int res = 10;
    for (int count = 1; count < exponent; count++) {
        res = res * 10;
    }
    return res;
}