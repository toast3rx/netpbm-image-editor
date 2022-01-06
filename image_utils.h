#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#include <stdio.h>
#include "structs.h"

/**
 * @brief Transpose NxN image of 2D array of pixels
 *
 * @param img image to be transposed
 * @param start_row starting position vertically
 * @param end_row ending position vertically
 * @param end_col ending position horizontally
 */
void transpose_square_matrix(image img, int start_row, int end_row, int end_col);

/**
 * @brief Transpose NxM image, 2D array of pixels
 *
 * @param img image to be transposed
 * @param height number of rows
 * @param width number of columns
 */
void transpose_image(image* img, int* height, int* width);

/**
 * @brief Reverse elements in every row for a selection of image
 *
 * @param img image where rows are reversed
 * @param start_row starting position vertically
 * @param end_row ending position vertically
 * @param start_col starting position horizontally
 * @param end_col ending position horizontally
 */
void reverse_rows(image img, int start_row, int end_row, int start_col, int end_col);

/**
 * @brief Reverse elements in every column for a selection of image
 *
 * @param start_row starting position vertically
 * @param end_row ending position vertically
 * @param start_col starting position horizontally
 * @param end_col ending position horizontally
 */
void reverse_cols(image img, int start_row, int end_row, int start_col, int end_col);

/**
 * @brief Free memory from image struct
 * 
 * @param img image to be freed
 * @param height number of rows
 */
void free_image(image img, int height);

/**
 * @brief Free memory from image and close its file
 * 
 * @param img netpbm image with all informations
 */
void free_all(netpbm_format img);

/**
 * @brief Rotate image clockwise
 * 
 * @param image image to be rotated
 * @param angle angle to be rotated by
 */
void rotate_image_clockwise(netpbm_format* image, int angle);

/**
 * @brief Rotate image counter-clockwise
 * 
 * @param image image to be rotated
 * @param angle angle to be rotated by
 */
void rotate_image_counter_clockwise(netpbm_format* image, int angle);

/**
 * @brief Rotate a selection clockwise from image by a certain angle
 * 
 * @param image netpbm image with all its informations
 * @param start_row starting position vertically
 * @param start_col starting position horizontally
 * @param end_row ending position vertically
 * @param end_col ending position horizontally
 * @param angle angle to be rotated by
 */
void rotate_selection_clockwise(netpbm_format* image, int start_row, int start_col, int end_row, int end_col, int angle);

/**
 * @brief Rotate a selection counter-clockwise from image by a certain image
 * 
 * @param image netpbm image with all its informations
 * @param start_row starting position vertically
 * @param start_col starting position horizontally
 * @param end_row ending position vertically
 * @param end_col ending position horizontally
 * @param angle angle to be rotated by
 */
void rotate_selection_counter_clockwise(netpbm_format* image, int start_row, int start_col, int end_row, int end_col, int angle);

/**
 * @brief Apply a given filter to image
 * 
 * @param img image to work with
 * @param kernel image kernel
 * @param fraction number to divide each element in kernel
 */
void apply_filter(netpbm_format* img, double kernel[3][3], double fraction);
#endif