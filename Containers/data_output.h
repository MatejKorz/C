#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct container_t;
struct path_t;
struct filter;
struct linked_node;
struct location;

/**
 * @brief Adds container_ID and pointer to neighbour_container to list of neighbours
 * 
 * @param container Container to add new neighbour to
 * @param neighbour_container Neighbouring container which will be added to neighbours
 * @retval true if neighbour is added successfully
 * @retval false if error occurs during adding
*/
bool add_neighbour(struct container_t *container, struct container_t *neighbour_container);

/**
 * @brief Finds container with given ID in array
 * 
 * @param container_array Array of pointers to linked nodes with ID and pointer to container
 * @param ID ID of wanted container
 * @param outside_index Here will be written index of founded container
 * @param container_array_length Lenght of container_ID_array
 * @retval true if container is found
 * @false if container is not in array
*/
bool find_container(struct linked_node *container_ID_array[], unsigned int ID, size_t *outside_index, size_t container_array_length);

/**
 * @brief Adds neighbours to all containers in container_array from paths_array
 * 
 * @param container_ID_array Array of pointers to linked nodes which contain ID and pointer to container
 * @param paths_array Array of pointers to paths structures
 * @param container_length Length of container_ID_array
 * @retval true if all neighbours are successfully added
 * @retval false is error occurs during adding
*/
bool add_all_neighbours(struct linked_node *container_ID_array[], struct path_t *path_array[], size_t container_length);

/**
 * @brief Adds container_ID to list of neighbours
 * 
 * @param location Location structure to add new neighbour to
 * @param location_ID ID to be added to location neighbours
 * @retval true if neighbour is added successfully
 * @retval false if error occurs during adding
*/
bool add_location_neighbour(struct location *location, unsigned int location_ID);

/**
 * @brief Adds neighbours to all locations in locations_array from containers_array
 * 
 * @param container_array Array of pointers to container structures
 * @param locations_array Array of pointers to location structures
 * @retval true if all neighbours are successfully added
 * @retval false is error occurs during adding
*/
void add_all_location_neighbours(struct container_t *container_array[], struct location *location_array[]);

/**
 * @brief Check sorted array to find duplicate IDs
 * 
 * @param container_ID_array Array of pointers to linked nodes which contain ID and pointer to container
 * @retval true if array does not contain duplicate IDs
 * @retval false if there is containers with duplicate IDs
*/
bool check_duplicate_ID(struct linked_node *container_ID_array[]);

/**
 * @brief Prints neighbours of given container
 * 
 * @param container Container structure
*/
void print_neighbours(struct container_t *container);

/**
 * @brief Prints all required infomration about container
 * 
 * @param container Container structure
*/
void print_container(struct container_t *container);

/**
 * @brief Checks if containers within the limits given by filter
 * 
 * @param container Pointer to container structure
 * @param filter Pointer to filter structure that specifies which containers to print
 * @retval true if container matches filter
 * @retval false if container doesnt match filter
*/
bool filter_container(struct container_t *container, struct filter *filter);

/**
 * @brief Prints all container from array using filter
 * 
 * @param container_array Array of pointers to container structures
 * @param filter Filter structure that specifies which containers to print
*/
void print_all_containers(struct container_t *container_array[], struct filter *filter);

/**
 * @brief Prints all required infomration about location
 * 
 * @param location Location structure
*/
void print_location(struct location *location);

/**
 * @brief Prints all locations from array
 * 
 * @param locations_array Array of pointers to location structures
*/
void print_all_locations(struct location *location_array[]);

/**
 * @brief Frees all data from given arrays
 * 
 * @param container_array Array of pointers to container structures
 * @param container_ID_array Array of nodes that contain container ID and poineter to container in container_array
 * @param path_array Array of pointers to path structures
 * @param location_array Array of pointers to location structures
*/
void delete_everything(struct container_t *container_array[], struct linked_node *container_ID_array[], struct path_t *path_array[], struct location *location_array[]);
