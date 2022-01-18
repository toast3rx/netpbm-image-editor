#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <math.h>

/**
 * @brief Split a string in words delimited by space
 *
 * @param str string to be split
 * @param words_array the array where the words will be stored
 */
void split_string(char *str, char **words_array);

/**
 * @brief Check for lines that start with #
 *  representing a commented line and skips it
 *  @param in - File to be checked
 */
void check_comments(FILE *in);

/**
 * @brief Dealocate memory a 2D array of char
 *
 * @param array Array to be deallocated
 * @param length number of rows
 */
void free_char_2D_array(char **array, int length);

/**
 * @brief Convert a string number to an int
 *
 * @param str number
 * @return int number converted
 */
int string_to_int(char *str);

/**
 * @brief Get the max object
 *
 * @param x
 * @param y
 * @return int
 */
int get_max(int x, int y);

/**
 * @brief Get the min object
 *
 * @param x
 * @param y
 * @return int
 */
int get_min(int x, int y);

/**
 * @brief Swap two numbers
 *
 * @param x
 * @param y
 */
void swap(int *x, int *y);

/**
 * @brief Restrict a number between a min and a max
 *
 * @param x number to be restricted
 * @param min value
 * @param max value
 */
void clamp(double *x, int min, int max);

/**
 * @brief Multiply every element in kernel with a given precision to
 *			move decimal digits to the right of the comma
 *
 * @param kernel values
 * @param n number of rows/cols
 * @param decimal_places number of decimal digits to be moved
 */
void move_decimals(double kernel[3][3], int n, int decimal_places);

#endif

