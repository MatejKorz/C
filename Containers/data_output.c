#include "data_output.h"

#include "containers.h"
#include "paths.h"

bool add_neighbour(struct container_t *container, struct container_t *neighbour_container)
{
    unsigned int neighbour_ID = neighbour_container->container_ID;
    struct linked_node *new_node = malloc(sizeof(struct linked_node));
    if (new_node == NULL) {
        return false;
    }
    initialize_node(new_node);
    new_node->ID = neighbour_ID;
    new_node->container_ptr = neighbour_container;

    if (container->neighbours == NULL) {
        container->neighbours = new_node;
    } else if (new_node->ID < container->neighbours->ID) {
        new_node->next = container->neighbours;
        container->neighbours = new_node;
    } else {
        struct linked_node *curr = container->neighbours;
        while (curr->next != NULL && curr->next->ID < new_node->ID) {
            curr = curr->next;
        }
        if (curr->ID == new_node->ID) {
            free(new_node);
        } else if (curr->next != NULL && curr->next->ID == new_node->ID) {
            free(new_node);
        } else {
            new_node->next = curr->next;
            curr->next = new_node;
        }
    }
    return true;
}

bool find_container(struct linked_node *container_ID_array[], unsigned int ID, size_t *outside_index, size_t container_array_length)
{
    long long low = 0;
    long long high = container_array_length;
    while (low <= high) {
        long long mid = (low + high) / 2;
        if (container_ID_array[mid]->ID == ID) {
            *outside_index = mid;
            return true;
        }
        if (container_ID_array[mid]->ID < ID) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return false;
}

bool add_all_neighbours(struct linked_node *container_ID_array[], struct path_t *path_array[], size_t container_length)
{
    size_t i = 0;
    while (path_array[i] != NULL) {
        size_t container_index = 0;
        if (!find_container(container_ID_array, path_array[i]->from_container, &container_index, container_length)) {
            fprintf(stderr, "Container (%u) found in paths but not in containers\n", path_array[i]->from_container);
            return false;
        }
        size_t to_container_index = 0;
        if (!find_container(container_ID_array, path_array[i]->to_container, &to_container_index, container_length)) {
            fprintf(stderr, "Container (%u) found in paths but not in containers\n", path_array[i]->to_container);
            return false;
        }
        add_neighbour(container_ID_array[container_index]->container_ptr, container_ID_array[to_container_index]->container_ptr);
        add_neighbour(container_ID_array[to_container_index]->container_ptr, container_ID_array[container_index]->container_ptr);
        i++;
    }

    return true;
}

bool add_location_neighbour(struct location *location, unsigned int location_ID)
{
    struct linked_node *new_node = malloc(sizeof(struct linked_node));
    if (new_node == NULL) {
        return false;
    }
    initialize_node(new_node);
    new_node->ID = location_ID;

    if (location->neighbours == NULL) {
        location->neighbours = new_node;
    } else if (new_node->ID < location->neighbours->ID) {
        new_node->next = location->neighbours;
        location->neighbours = new_node;
    } else {
        struct linked_node *curr = location->neighbours;
        while (curr->next != NULL && curr->next->ID < new_node->ID) {
            curr = curr->next;
        }
        if (curr->ID == new_node->ID) {
            free(new_node);
        } else if (curr->next != NULL && curr->next->ID == new_node->ID) {
            free(new_node);
        } else {
            new_node->next = curr->next;
            curr->next = new_node;
        }
    }
    return true;
}

void add_all_location_neighbours(struct container_t *container_array[], struct location *location_array[])
{
    size_t index = 0;
    while (container_array[index] != NULL) {
        struct container_t *curr_container = container_array[index];
        struct linked_node *curr = curr_container->neighbours;
        while (curr != NULL) {
            if (curr->container_ptr->location_ID != curr_container->location_ID) {
                size_t location_index = curr_container->location_ID;
                add_location_neighbour(location_array[location_index], curr->container_ptr->location_ID);
            }
            curr = curr->next;
        }
        index++;
    }
}

bool check_duplicate_ID(struct linked_node *container_ID_array[])
{
    if (container_ID_array[0] == NULL) {
        return true;
    }
    size_t index = 1;
    while (container_ID_array[index] != NULL) {
        if (container_ID_array[index - 1]->ID == container_ID_array[index]->ID) {
            fprintf(stderr, "Duplicate container ID(%u)\n", container_ID_array[index]->ID);
            return false;
        }
        index++;
    }
    return true;
}

const char *garbage_type_to_string(enum garbage_t garbage_type)
{
    switch (garbage_type) {
    case PLASTIC:
        return "Plastics and Aluminium";
    case PAPER:
        return "Paper";
    case BIODEGRADABLE:
        return "Biodegradable waste";
    case CLEAR_GLASS:
        return "Clear glass";
    case COLORED_GLASS:
        return "Colored glass";
    case TEXTILE:
        return "Textile";
    default:
        return NULL;
    }
}

bool filter_container(struct container_t *container, struct filter *filter)
{
    if (filter->agrument_types[TYPE]) {
        if (!(filter->garbage_types[container->garbage_type])) {
            return false;
        }
    }
    if (filter->agrument_types[VOLUME]) {
        if (!(container->capacity >= filter->volume_min && container->capacity <= filter->volume_max)) {
            return false;
        }
    }
    if (filter->agrument_types[PUBLIC]) {
        if (container->is_public != filter->show_public) {
            return false;
        }
    }
    if (filter->agrument_types[LOCATION]) {
    }
    return true;
}

void print_neighbours(struct container_t *container)
{
    struct linked_node *curr = container->neighbours;
    while (curr != NULL) {
        printf(" %u", curr->ID);
        curr = curr->next;
    }
}

void print_container(struct container_t *container)
{
    printf("ID: %u, ", container->container_ID);
    printf("Type: %s, ", garbage_type_to_string(container->garbage_type));
    printf("Capacity: %u, ", container->capacity);
    printf("Address: ");
    if (container->street_name != NULL) {
        printf("%s", container->street_name);
        if (container->street_num != 0) {
            printf(" ");
        }
    }
    if (container->street_num != 0) {
        printf("%u", container->street_num);
    }
    printf(", ");
    printf("Neighbors:");
    print_neighbours(container);
    printf("\n");
}

void print_all_containers(struct container_t *container_array[], struct filter *filter)
{
    size_t index = 0;
    while (container_array[index] != NULL) {
        if (filter_container(container_array[index], filter)) {
            print_container(container_array[index]);
        }
        index++;
    }
}

void print_garbage_types(struct location *location)
{
    if (location->garbage_types[PLASTIC]) {
        printf("A");
    }
    if (location->garbage_types[PAPER]) {
        printf("P");
    }
    if (location->garbage_types[BIODEGRADABLE]) {
        printf("B");
    }
    if (location->garbage_types[CLEAR_GLASS]) {
        printf("G");
    }
    if (location->garbage_types[COLORED_GLASS]) {
        printf("C");
    }
    if (location->garbage_types[TEXTILE]) {
        printf("T");
    }
}

void print_location(struct location *location)
{
    printf("%u;", location->ID + 1);
    print_garbage_types(location);
    printf(";");
    struct linked_node *curr = location->neighbours;
    while (curr != NULL) {
        if (curr->next == NULL) {
            printf("%u", curr->ID + 1);
        } else {
            printf("%u,", curr->ID + 1);
        }
        curr = curr->next;
    }
    printf("\n");
}

void print_all_locations(struct location *location_array[])
{
    size_t index = 0;
    while (location_array[index] != NULL) {
        print_location(location_array[index]);
        index++;
    }
}

void delete_everything(struct container_t *container_array[], struct linked_node *container_ID_array[], struct path_t *path_array[], struct location *location_array[])
{
    delete_all_containers(container_array);
    delete_all_ID_containers(container_ID_array);
    delete_all_paths(path_array);
    delete_all_locations(location_array);
}