#ifndef COMMAND_CHECK_H
#define COMMAND_CHECK_H

/**
 * @brief
 * Check if inserted command is the LOAD command
 * @param command input from stdin
 * @return 1 for true, 0 for false
 */
int is_load_command(char *command);

/**
 * @brief Check for select command
 *
 * @param command given by the user
 * @return int true or false
 */
int is_select_command(char *command);

/**
 * @brief Check for select all command
 *
 * @param command given by the user
 * @param arg second argument
 * @return int true or false
 */
int is_select_all_command(char *command, char *arg);

/**
 * @brief Check if command given by user is for rotating the image
 *
 * @param args list of arguments given by the user:
 *	first is the command and second the angle
 * @param length number of arguments
 * @return int true or false
 */
int is_rotate_command(char **args, int length);

/**
 * @brief Check if command given by the user is for exit the program
 *
 * @param command given by user
 * @param img image with all properties to check for file load
 * @return int true or false
 */
int is_exit_command(char *command, netpbm_format img);

/**
 * @brief Check if command given by the user is for cropping the image
 *
 * @param command given by the user
 * @return int true or false
 */
int is_crop_command(char *command);

/**
 * @brief Check if command given by user is for applying filter
 *
 * @param command given by user
 * @return int true or false
 */
int is_apply_filter_command(char *command);

/**
 * @brief Check if command given by user is for saving the image
 *
 * @param command given by user
 * @return int true or false
 */
int is_save_command(char *command);

#endif
