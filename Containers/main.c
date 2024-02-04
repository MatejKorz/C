#include "containers.h"
#include "data_output.h"
#include "data_source.h"
#include "paths.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct arrays_t
{
    size_t con_length;
    size_t path_length;

    struct container_t **containers;
    struct path_t **paths;
    struct linked_node **containers_ID;
    struct location **locations;
};

void initialize_array_info(struct arrays_t *array_info)
{
    array_info->con_length = number_of_containers();
    array_info->path_length = number_of_paths();

    array_info->containers = NULL;
    array_info->paths = NULL;
    array_info->containers_ID = NULL;
    array_info->locations = NULL;
}

void initialize_filter(struct filter *arg_filter)
{
    memset(arg_filter->agrument_types, false, sizeof(bool) * ARGUMENT_T_LENGTH);
    memset(arg_filter->garbage_types, false, sizeof(bool) * GARBAGE_T_LENGTH);
    arg_filter->volume_min = -1;
    arg_filter->volume_max = -1;
    arg_filter->show_public = false;
}

int argument_type_check(char argument[])
{
    if (argument[0] != '-' || argument[2] != '\0') {
        fprintf(stderr, "Invalid argument format/missing specifiers\n");
        return EXIT_FAILURE;
    }
    switch (argument[1]) {
    case 't':
        return TYPE;
        break;
    case 'c':
        return VOLUME;
        break;
    case 'p':
        return PUBLIC;
        break;
    case 's':
        return LOCATION;
        break;
    default:
        fprintf(stderr, "Invalid argument type '%s'\n", argument);
        return EXIT_FAILURE;
    }
}

int argument_uniques_check(enum argument_t arg_type, struct filter *arg_filter)
{
    if (arg_filter->agrument_types[arg_type]) {
        fprintf(stderr, "Multiple of same argument\n");
        return EXIT_FAILURE;
    }
    if (arg_type != LOCATION && arg_filter->agrument_types[LOCATION]) {
        fprintf(stderr, "Arguments from different tasks\n");
        return EXIT_FAILURE;
    }

    if (arg_type == LOCATION) {
        for (int i = TYPE; i < ARGUMENT_T_LENGTH - 1; i++) {
            if (arg_filter->agrument_types[i]) {
                fprintf(stderr, "Arguments from different tasks\n");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}

int parse_garbage_type(char argument[], struct filter *arg_filter)
{
    char *ptr = argument;
    if (argument[0] == '\0') {
        fprintf(stderr, "Garbage type not specified\n");
        return EXIT_FAILURE;
    }

    while (*ptr != '\0') {
        switch (*ptr) {
        case 'A':
            arg_filter->garbage_types[PLASTIC] = true;
            break;
        case 'P':
            arg_filter->garbage_types[PAPER] = true;
            break;
        case 'B':
            arg_filter->garbage_types[BIODEGRADABLE] = true;
            break;
        case 'G':
            arg_filter->garbage_types[CLEAR_GLASS] = true;
            break;
        case 'C':
            arg_filter->garbage_types[COLORED_GLASS] = true;
            break;
        case 'T':
            arg_filter->garbage_types[TEXTILE] = true;
            break;
        default:
            fprintf(stderr, "Unknown garbage type '%s'", ptr);
            return EXIT_FAILURE;
        }
        ptr++;
    }
    return EXIT_SUCCESS;
}

int parse_volume(char argument[], struct filter *arg_filter)
{
    if (argument[0] == '\0') {
        fprintf(stderr, "Volume not specified\n");
        return EXIT_FAILURE;
    }

    const char *delimiter = "-";
    char *token = strtok(argument, delimiter);
    while (*token != '\0') {
        token++;
    }
    token++;
    if (token == NULL || !is_digit(argument) || !is_digit(token)) {
        fprintf(stderr, "Invalid volume format\n");
        return EXIT_FAILURE;
    }

    unsigned int minimum = strtoul(argument, NULL, 10);
    unsigned int maximum = strtoul(token, NULL, 10);
    if (maximum < minimum) {
        fprintf(stderr, "Invalid volume format\n");
        return EXIT_FAILURE;
    }

    arg_filter->volume_min = minimum;
    arg_filter->volume_max = maximum;

    return EXIT_SUCCESS;
}

int parse_public_access(char argument[], struct filter *arg_filter)
{
    if (argument[1] != '\0') {
        fprintf(stderr, "Invalid public specifier\n");
        return EXIT_FAILURE;
    }
    switch (*argument) {
    case 'Y':
        arg_filter->show_public = true;
        break;
    case 'N':
        arg_filter->show_public = false;
        break;
    default:
        fprintf(stderr, "Invalid public specifier\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int arguments_parser(int argc, char *argv[], struct filter *arg_filter)
{
    int i = 1;
    enum argument_t arg_type;
    while (i < argc - 2) {
        arg_type = argument_type_check(argv[i]);
        if (argument_uniques_check(arg_type, arg_filter) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        switch (arg_type) {
        case TYPE:
            if (i + 1 >= argc - 2) {
                fprintf(stderr, "Garbage type not specified\n");
                return EXIT_FAILURE;
            }
            if (parse_garbage_type(argv[i + 1], arg_filter) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            };
            arg_filter->agrument_types[TYPE] = true;
            i++;
            break;
        case VOLUME:
            if (i + 1 >= argc - 2) {
                fprintf(stderr, "Volume not specified\n");
                return EXIT_FAILURE;
            }
            if (parse_volume(argv[i + 1], arg_filter) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            };
            arg_filter->agrument_types[VOLUME] = true;
            i++;
            break;
        case PUBLIC:
            if (i + 1 >= argc - 2) {
                fprintf(stderr, "Missing public specifier\n");
                return EXIT_FAILURE;
            }
            if (parse_public_access(argv[i + 1], arg_filter) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }
            arg_filter->agrument_types[PUBLIC] = true;
            i++;
            break;
        case LOCATION:
            arg_filter->agrument_types[LOCATION] = true;
            break;
        default:
            fprintf(stderr, "Argument parsing failed\n");
            return EXIT_FAILURE;
        }
        i++;
    }

    return EXIT_SUCCESS;
}

int load_all_from_data_source(struct arrays_t *array_info)
{
    if (array_info->con_length == 0 && array_info->path_length != 0) {
        destroy_data_source();
        return EXIT_FAILURE;
    }
    if (array_info->con_length == 0 && array_info->path_length == 0) {
        destroy_data_source();
        return EXIT_SUCCESS;
    }

    if (!load_all_containers(array_info->containers, array_info->containers_ID, array_info->con_length, array_info->locations)) {
        delete_everything(array_info->containers, array_info->containers_ID, array_info->paths, array_info->locations);
        destroy_data_source();
        return EXIT_FAILURE;
    }

    if (array_info->path_length != 0 && !load_all_paths(array_info->paths, array_info->path_length)) {
        delete_everything(array_info->containers, array_info->containers_ID, array_info->paths, array_info->locations);
        destroy_data_source();
        return EXIT_FAILURE;
    }

    destroy_data_source();
    return EXIT_SUCCESS;
}

int output_data(struct arrays_t *array_info, struct filter *arg_filter)
{
    if (!check_duplicate_ID(array_info->containers_ID)) {
        delete_everything(array_info->containers, array_info->containers_ID, array_info->paths, array_info->locations);
        return EXIT_FAILURE;
    }

    if (!add_all_neighbours(array_info->containers_ID, array_info->paths, array_info->con_length)) {
        delete_everything(array_info->containers, array_info->containers_ID, array_info->paths, array_info->locations);
        return EXIT_FAILURE;
    }

    if (arg_filter->agrument_types[LOCATION]) {
        add_all_location_neighbours(array_info->containers, array_info->locations);
        print_all_locations(array_info->locations);
    } else {
        print_all_containers(array_info->containers, arg_filter);
    }

    delete_everything(array_info->containers, array_info->containers_ID, array_info->paths, array_info->locations);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    struct filter arg_filter;
    initialize_filter(&arg_filter);

    if (argc < 3) {
        fprintf(stderr, "Not enough arguments provided\n");
        return EXIT_FAILURE;
    }
    if (!init_data_source(argv[argc - 2], argv[argc - 1])) {
        fprintf(stderr, "Initializing data source failed\n");
        return EXIT_FAILURE;
    }

    if (arguments_parser(argc, argv, &arg_filter) == EXIT_FAILURE) {
        destroy_data_source();
        return EXIT_FAILURE;
    }

    struct arrays_t array_info;
    initialize_array_info(&array_info);

    struct container_t *container_array[array_info.con_length + 1];
    memset(container_array, 0, sizeof(struct container_t *) * (array_info.con_length + 1));

    struct linked_node *container_ID_array[array_info.con_length + 1];
    memset(container_ID_array, 0, sizeof(struct linked_node *) * (array_info.con_length + 1));

    struct path_t *path_array[array_info.path_length + 1];
    memset(path_array, 0, sizeof(struct path_t *) * (array_info.path_length + 1));

    struct location *location_array[array_info.con_length + 1];
    memset(location_array, 0, sizeof(struct location_t *) * (array_info.con_length + 1));

    array_info.containers = container_array;
    array_info.paths = path_array;
    array_info.containers_ID = container_ID_array;
    array_info.locations = location_array;

    if (load_all_from_data_source(&array_info) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    qsort(container_ID_array, array_info.con_length, sizeof(struct linked_node *), node_compare_ID);
    qsort(path_array, array_info.path_length, sizeof(struct path_t *), path_compare_from);

    if (output_data(&array_info, &arg_filter) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
