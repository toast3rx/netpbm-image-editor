#ifndef COMMANDS_H
#define COMMANDS_H
#include "structs.h"
/**
 * @brief Read a command from stdin
 *
 * @param command command to be read
 * @return arguments of the command
 */
char** read_command(char* command, int* args);

/**
 * @brief Load an image from stdin
 *
 * @param file_name File name given by user
 * @param image netpbm struct that stores informations about the file
 */
void load_file(char* file_name, netpbm_format* netpbm_img);

/**
 * @brief Restrict current selection to the given index
 *
 * @param args array with command arguments
 * @param image netpbm image
 */
void select_command(char** args, netpbm_format* image);

/**
 * @brief Select all image as current selection
 *
 * @param img netpbm image
 */
void select_all_command(netpbm_format *img);

/**
 * @brief Rotate netpbm image based on current selection
 *  with a given angle
 * 
 * @param image netpbm image with all its information
 * @param angle angle from user input 
 */
void rotate_command(netpbm_format* image, char* angle);

/**
 * @brief Crop image to image selection
 * 
 * @param img netpbm img with all its information
 */
void crop_command(netpbm_format* img);

/**
 * @brief Apply filter 
 * 
 * @param img image to work with
 * @param filter filter from user input
 */
void apply_command(netpbm_format* img, char* filter);

/**
 * @brief Save image in a new file
 * 
 * @param img image to be saved
 * @param file_name new file name
 * @param is_ascii save image as ascii or binary
 */
void save_command(netpbm_format img, char* file_name, char* is_ascii);

#endif