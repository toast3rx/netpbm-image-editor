#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdio.h>

/**
 * @brief Types of image this application can process
 *
 */
typedef enum
{
    PPM,
    PGM
} image_type;

/**
 * @brief Pixel struct that stores red, green and blue values
 *
 */
typedef struct
{
    unsigned int red, green, blue;
} pixel;

/**
 * @brief PPM image with an array of pixels
 *
 */
typedef struct
{
    pixel** pixels;
} image;

/**
 * @brief A geomtrical point with x/y coordinates
 *
 */
typedef struct
{
    int x;
    int y;
} point;

/**
 * @brief Select a part of the image given by the coords placed diag opposed
 *
 */
typedef struct
{
    point p1;
    point p2;
} selection;

/**
 * @brief NetPbm image
 *
 */
typedef struct
{
    FILE* file_in;
    image_type type;
    char magic_number[3];
    int width;
    int height;
    int max_value;
    image picture;
    selection curr_selection;
} netpbm_format;

#endif