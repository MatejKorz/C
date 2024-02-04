#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct path_t
{
    unsigned int from_container;
    unsigned int to_container;
    unsigned int distance;
};

/**
 * @brief Initializes path structure
 * 
 * @param path Pointer to path structure to initialize
*/
void initialize_path(struct path_t *path);

/**
 * @brief Loads value of unsigned int to path structer parameter
 * 
 * @param path_uint Pointer to path structure parameter, where value will be loaded
 * @param line_index Number of wanted line
 * @param funcion Funcion from data_source.h, which returs string on line_index
 * @retval true if retval of function is successfully converted to unsigned int and loaded to path
 * @retval false if value cant be loaded
*/
bool load_path_uint(unsigned int *path_uint, size_t line_index, const char *function(size_t line_index));

/**
 * @brief Load all parameters of path structure
 * 
 * @param path Pointer to path structure to load to
 * @param line_index Line of file, from where parameters will be loading
 * @retval true if all values are loaded successfully
 * @retval false if one of values cant be loaded
*/
bool load_path(struct path_t *path, size_t line_index);

/**
 * @brief Finds out, if paths file contains data on given line
 * 
 * @param line_index Line number of paths file
 * @retval true if line contains data
 * @retval false if line is empty
*/
bool path_in_range(size_t line_index);

/**
 * @brief Counts number of lines in paths file
 * 
 * @retval Number of lines in paths file
*/
size_t number_of_paths();

/**
 * @brief Loads all data from paths file to array of path structures
 * 
 * @param path_array Array of poinetrs to path structres, initialized to NULL
 * @param array_length Length of path_array
 * @retval true if all data is successfully loaded
 * @retval false if all data cant be loaded
*/
bool load_all_paths(struct path_t *path_array[], size_t array_length);

/**
 * @brief Compares parameter from_container in qsort
*/
int path_compare_from(const void *path1_ptr, const void *path2_ptr);

/**
 * @breif Frees all path structures in array
 * 
 * @param path_array Array of poinetrs to path structres
*/
void delete_all_paths(struct path_t *path_array[]);