#include "containers.h"

#include "data_source.h"

void initialize_container(struct container_t *container)
{
    container->container_ID = 0;
    memset(container->coordinates, 0, sizeof(double) * 2);
    container->garbage_type = -1;
    container->capacity = 0;
    container->is_public = false;
    container->neighbours = NULL;
    container->location_ID = 0;
    // OPTIONAL PARAMETERS
    container->name = NULL;
    container->street_name = NULL;
    container->street_num = 0;
}

void initialize_location(struct location *location)
{
    location->ID = 0;
    memset(location->coordinates, 0, sizeof(double) * 2);
    memset(location->garbage_types, false, sizeof(bool) * GARBAGE_T_LENGTH);
    location->neighbours = NULL;
}

void initialize_node(struct linked_node *node)
{
    node->ID = 0;
    node->next = NULL;
    node->container_ptr = NULL;
}

bool is_digit(const char *string)
{
    for (size_t i = 0; i < strlen(string); i++) {
        if (isdigit(string[i]) == 0) {
            return false;
        }
    }
    return true;
}

int node_compare_ID(const void *node1_ptr, const void *node2_ptr)
{
    struct linked_node **node1 = (struct linked_node **) node1_ptr;
    struct linked_node **node2 = (struct linked_node **) node2_ptr;

    if ((*node1)->ID > (*node2)->ID) {
        return 1;
    }
    if ((*node1)->ID < (*node2)->ID) {
        return -1;
    }
    return 0;
}

bool load_container_id(struct container_t *container, size_t line_index)
{
    const char *id_string = get_container_id(line_index);
    if (!is_digit(id_string)) {
        return false;
    }
    unsigned int id = strtoul(id_string, NULL, 10);
    if (id == UINT_MAX || errno == ERANGE) {
        return false;
    }
    container->container_ID = id;
    return true;
}

bool load_container_coordinates(struct container_t *container, size_t line_index)
{
    const char *x_string = get_container_x(line_index);
    const char *y_string = get_container_y(line_index);
    char *decimal_ptr = strstr(x_string, ".");

    if (decimal_ptr == NULL || !is_digit(decimal_ptr + 1) || strlen(decimal_ptr) > 16) {
        return false;
    }
    decimal_ptr = strstr(y_string, ".");
    if (decimal_ptr == NULL || !is_digit(decimal_ptr + 1) || strlen(decimal_ptr) > 16) {
        return false;
    }

    double x = strtod(x_string, NULL);
    double y = strtod(y_string, NULL);
    if ((x == 0) || (y == 0)) {
        return false;
    }
    container->coordinates[0] = x;
    container->coordinates[1] = y;
    return true;
}

bool load_container_garbage_type(struct container_t *container, size_t line_index)
{
    const char *garbage_type_string = get_container_waste_type(line_index);
    if (strcmp(garbage_type_string, "Plastics and Aluminium") == 0) {
        container->garbage_type = PLASTIC;
    } else if (strcmp(garbage_type_string, "Paper") == 0) {
        container->garbage_type = PAPER;
    } else if (strcmp(garbage_type_string, "Biodegradable waste") == 0) {
        container->garbage_type = BIODEGRADABLE;
    } else if (strcmp(garbage_type_string, "Clear glass") == 0) {
        container->garbage_type = CLEAR_GLASS;
    } else if (strcmp(garbage_type_string, "Colored glass") == 0) {
        container->garbage_type = COLORED_GLASS;
    } else if (strcmp(garbage_type_string, "Textile") == 0) {
        container->garbage_type = TEXTILE;
    } else {
        return false;
    }
    return true;
}

bool load_container_capacity(struct container_t *container, size_t line_index)
{
    const char *capacity_string = get_container_capacity(line_index);
    if (strcmp(capacity_string, "\0") == 0 || !is_digit(capacity_string)) {
        return false;
    }
    unsigned int capacity = strtoul(capacity_string, NULL, 10);
    if (capacity == UINT_MAX || errno == ERANGE) {
        return false;
    }
    container->capacity = capacity;
    return true;
}

bool load_container_public(struct container_t *container, size_t line_index)
{
    const char *public_string = get_container_public(line_index);
    if (strcmp(public_string, "Y") == 0) {
        container->is_public = true;
        return true;
    }
    if (strcmp(public_string, "N") == 0) {
        container->is_public = false;
        return true;
    }
    return false;
}

void load_container_name(struct container_t *container, size_t line_index)
{
    const char *name_string = get_container_name(line_index);
    if (strcmp(name_string, "") != 0) {
        container->name = malloc(strlen(name_string) + 1);
        strcpy(container->name, name_string);
    }
}

void load_container_street_name(struct container_t *container, size_t line_index)
{
    const char *street_string = get_container_street(line_index);
    if (strcmp(street_string, "") != 0) {
        container->street_name = malloc(strlen(street_string) + 1);
        strcpy(container->street_name, street_string);
    }
}

bool load_container_street_num(struct container_t *container, size_t line_index)
{
    const char *street_num_string = get_container_number(line_index);
    if (street_num_string != NULL) {
        if (!is_digit(street_num_string)) {
            return false;
        }
        unsigned int street_num = strtoul(street_num_string, NULL, 10);
        if (street_num == UINT_MAX || errno == ERANGE) {
            return false;
        }
        container->street_num = street_num;
    }
    return true;
}

bool container_in_range(size_t line_index)
{
    return get_container_id(line_index) != NULL;
}

size_t number_of_containers()
{
    size_t count = 0;
    while (container_in_range(count)) {
        count++;
    }
    return count;
}

bool load_container(struct container_t *container, size_t line_index)
{
    initialize_container(container);

    if (!load_container_id(container, line_index)) {
        fprintf(stderr, "Inavlid container ID on line %lu\n", line_index + 1);
        return false;
    }

    if (!load_container_coordinates(container, line_index)) {
        fprintf(stderr, "Inavlid container coordinates on line %lu\n", line_index + 1);
        return false;
    }

    if (!load_container_garbage_type(container, line_index)) {
        fprintf(stderr, "Inavlid container garbage type on line %lu\n", line_index + 1);
        return false;
    }

    if (!load_container_capacity(container, line_index)) {
        fprintf(stderr, "Inavlid container capacity on line %lu\n", line_index + 1);
        return false;
    }

    if (!load_container_public(container, line_index)) {
        fprintf(stderr, "Inavlid container public on line %lu\n", line_index + 1);
        return false;
    }

    //optional parameters
    if (!load_container_street_num(container, line_index)) {
        fprintf(stderr, "Inavlid container street number on line %lu\n", line_index + 1);
        return false;
    }

    load_container_name(container, line_index);
    load_container_street_name(container, line_index);

    return true;
}

void delete_container(struct container_t *container)
{
    if (container->name != NULL) {
        free(container->name);
    }
    if (container->street_name != NULL) {
        free(container->street_name);
    }
    if (container->neighbours != NULL) {
        struct linked_node *curr = container->neighbours;
        while (curr != NULL) {
            struct linked_node *delete_node = curr;
            curr = curr->next;
            free(delete_node);
        }
    }
    free(container);
}

bool load_all_containers(struct container_t *container_array[], struct linked_node *container_ID_array[], size_t array_length, struct location *location_array[])
{
    size_t container_count = 0;
    while (container_count < array_length) {
        struct container_t *new_container = malloc(sizeof(struct container_t));
        if (new_container == NULL) {
            fprintf(stderr, "Container allocation failed\n");
            return false;
        }
        if (!load_container(new_container, container_count)) {
            free(new_container);
            return false;
        }
        size_t location_index = find_location(new_container->coordinates, location_array);
        if (location_array[location_index] == NULL) {
            location_array[location_index] = malloc(sizeof(struct location));
            if (location_array[location_index] == NULL) {
                fprintf(stderr, "Location allocation failed\n");
                free(new_container);
                return false;
            }
            initialize_location(location_array[location_index]);
            location_array[location_index]->ID = location_index;
            location_array[location_index]->coordinates[0] = new_container->coordinates[0];
            location_array[location_index]->coordinates[1] = new_container->coordinates[1];
        }
        location_array[location_index]->garbage_types[new_container->garbage_type] = true;
        new_container->location_ID = location_index;

        struct linked_node *ID_node = malloc(sizeof(struct linked_node));
        if (ID_node == NULL) {
            fprintf(stderr, "ID node allocation failed\n");
            free(new_container);
            free(location_array[location_index]);
            return false;
        }
        initialize_node(ID_node);
        ID_node->ID = new_container->container_ID;
        ID_node->container_ptr = new_container;
        container_ID_array[container_count] = ID_node;

        container_array[container_count] = new_container;
        container_count++;
    }

    return true;
}

void delete_all_containers(struct container_t *container_array[])
{
    size_t index = 0;
    while (container_array[index] != NULL) {
        delete_container(container_array[index]);
        index++;
    }
}

size_t find_location(double coordinates[], struct location *location_array[])
{
    size_t index = 0;
    while (location_array[index] != NULL) {
        if (location_array[index]->coordinates[0] == coordinates[0] && location_array[index]->coordinates[1] == coordinates[1]) {
            return index;
        }
        index++;
    }
    return index;
}

void delete_all_locations(struct location *location_array[])
{
    size_t index = 0;
    while (location_array[index] != NULL) {
        if (location_array[index]->neighbours != NULL) {
            struct linked_node *curr = location_array[index]->neighbours;
            while (curr != NULL) {
                struct linked_node *delete_node = curr;
                curr = curr->next;
                free(delete_node);
            }
        }
        free(location_array[index]);
        index++;
    }
}

void delete_all_ID_containers(struct linked_node *container_ID_array[])
{
    size_t index = 0;
    while (container_ID_array[index] != NULL) {
        free(container_ID_array[index]);
        index++;
    }
}