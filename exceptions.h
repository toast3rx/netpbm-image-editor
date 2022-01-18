#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/**
 * @brief Print error message when there is no file loaded
 *
 */
void no_file_load_exception(void);

/**
 * @brief Print error message when there is an invalid command
 *
 */
void invalid_command_exception(void);

/**
 * @brief Print error message when rotating angle is invalid
 *
 */
void invalid_angle_exception(void);

/**
 * @brief Print error message for memory allocation error
 *
 */
void memory_allocation_exception(void);

/**
 * @brief Print error message when coordinates are invalid
 *
 */
void invalid_coord_exception(void);

/**
 * @brief Print error message when the selection is not square
 *
 */
void square_selection_exception(void);

/**
 * @brief Print error message when the file cannot be opened
 *
 * @param file_name name of the file
 */
void open_file_exception(char *file_name);

/**
 * @brief Print error message when apply paramter is invalid
 *
 */
void apply_parameter_exception(void);

/**
 * @brief Print error message when file name is not inserted
 *
 */
void no_file_name_exception(void);

#endif
