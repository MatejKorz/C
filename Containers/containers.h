#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum argument_t
{
    TYPE,
    VOLUME,
    PUBLIC,
    LOCATION,
    ARGUMENT_T_LENGTH
};

enum garbage_t
{
    PLASTIC,
    PAPER,
    BIODEGRADABLE,
    CLEAR_GLASS,
    COLORED_GLASS,
    TEXTILE,
    GARBAGE_T_LENGTH
};

struct filter
{
    bool agrument_types[ARGUMENT_T_LENGTH];
    bool garbage_types[GARBAGE_T_LENGTH];
    unsigned int volume_min;
    unsigned int volume_max;
    bool show_public;
};

struct container_t
{
    unsigned int container_ID;
    double coordinates[2];
    enum garbage_t garbage_type;
    unsigned int capacity;
    bool is_public;
    struct linked_node *neighbours;
    unsigned int location_ID;
    // OPTIONAL PARAMETERS
    char *name;
    char *street_name;
    unsigned int street_num;
};

struct linked_node
{
    unsigned int ID;
    struct linked_node *next;

    struct container_t *container_ptr;
};

struct location
{
    unsigned int ID;
    double coordinates[2];
    bool garbage_types[GARBAGE_T_LENGTH];
    struct linked_node *neighbours;
};

/**
 * @brief Initializes container structure
 * 
 * @param path Pointer to container structure to initialize
*/
void initialize_container(struct container_t *container);

/**
 * @brief Initializes location structure
 * 
 * @param path Pointer to location structure to initialize
*/
void initialize_location(struct location *location);

/**
 * @brief Initializes node structure
 * 
 * @param path Pointer to node structure to initialize
*/
void initialize_node(struct linked_node *node);

/**
 * @brief Check if string containes only digits
 * 
 * @param string String to check
 * @retval true if string containes only digits
 * @retval false otherwise
*/
bool is_digit(const char *string);

/**
 * @brief Compares parameter ID in linekd_node in qsort
*/
int node_compare_ID(const void *node1_ptr, const void *node2_ptr);

/**
 * @brief Loads container_ID parameter in container from specified line in containers file
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where value will load
 * @retval true if value is loaded successfully
 * @retval false if value is invalid
*/
bool load_container_id(struct container_t *container, size_t line_index);

/**
 * @brief Loads coordinates parameter in container from specified line in containers file
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where values will load
 * @retval true if values are loaded successfully
 * @retval false if value is invalid
*/
bool load_container_coordinates(struct container_t *container, size_t line_index);

/**
 * @brief Loads garbage parameter in container from specified line in containers file
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where value will load
 * @retval true if value is loaded successfully
 * @retval false if value is invalid
*/
bool load_container_garbage_type(struct container_t *container, size_t line_index);

/**
 * @brief Loads capacity parameter in container from specified line in containers file
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where value will load
 * @retval true if value is loaded successfully
 * @retval false if value is invalid
*/
bool load_container_capacity(struct container_t *container, size_t line_index);

/**
 * @brief Loads is_pubic parameter in container from specified line in containers file
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where value will load
 * @retval true if value is loaded successfully
 * @retval false if value is invalid
*/
bool load_container_public(struct container_t *container, size_t line_index);

/**
 * @brief Loads name parameter in container from specified line in containers file
 * 
 * @note if value cant be loaded, name parameter will stay at initial value
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where value will load
*/
void load_container_name(struct container_t *container, size_t line_index);

/**
 * @brief Loads street_name parameter in container from specified line in containers file
 * 
 * @note if value cant be loaded, street_name parameter will stay at initial value
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where value will load
*/
void load_container_street_name(struct container_t *container, size_t line_index);

/**
 * @brief Loads street_num parameter in container from specified line in containers file
 * 
 * @note if value cant be loaded, street_num  parameter will stay at initial value
 * 
 * @param container Pointer to container structure
 * @param line_index Numbers of line from where value will load
 * @retval true if value is load successfully
 * @retval false if value is invalid
*/
bool load_container_street_num(struct container_t *container, size_t line_index);

/**
 * @brief Finds out, if containers file contains data on given line
 * 
 * @param line_index Line number of paths file
 * @retval true if line contains data
 * @retval false if line is empty
*/
bool container_in_range(size_t line_index);

/**
 * @brief Counts number of lines in paths file
 * 
 * @retval Number of lines in paths file
*/
size_t number_of_containers();

/**
 * @brief Load all parameters of container structure
 * 
 * @param container Pointer to container structure to load to
 * @param line_index Line of file, from where parameters will be loading
 * @retval true if all values are loaded successfully
 * @retval false if one of values cant be loaded
*/
bool load_container(struct container_t *container, size_t line_index);

/**
 * @breif Frees all allocated data inside container structure and container itself
 * 
 * @param container Pointer to container structure to be freed
*/
void delete_container(struct container_t *container);

/**
 * @brief Loads all data from containers file to array of container structures
 * 
 * @param container_array Array of poinetrs to container structres, initialized to NULL
 * @param array_length Length of container_array
 * @param container_ID_array Array of nodes that contain container ID and poineter to container in container_array
 * @param location_array Array of pointers to location structure
 * @retval true if all data is successfully loaded
 * @retval false if all data cant be loaded
*/
bool load_all_containers(struct container_t *container_array[], struct linked_node *container_ID_array[], size_t array_length, struct location *location_array[]);

/**
 * @breif Frees all container structures in array
 * 
 * @param container_array Array of poinetrs to container structres
*/
void delete_all_containers(struct container_t *container_array[]);

/**
 * @breif Finds index of location structure with given coordinates
 * 
 * @param coordinates Array of doubles - X, Y coordinates
 * @param location_array Array of pointers to location structure
 * @retval index of pointer to structure with matching coordinates, if not found index of last initialized location + 1
*/
size_t find_location(double coordinates[], struct location *location_array[]);

/**
 * @breif Frees all location structures in array
 * 
 * @param location_array Array of poinetrs to container structres
*/
void delete_all_locations(struct location *location_array[]);

/**
 * @brief Frees all nodes from container_ID array
 * 
 * @param container_ID_array Array of nodes that contain container ID and poineter to container in container_array
*/
void delete_all_ID_containers(struct linked_node *container_ID_array[]);
