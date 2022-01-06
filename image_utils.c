#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "image_utils.h"
#include "utils.h"

void transpose_square_matrix(image img, int start_row, int end_row, int end_col)
{
    for (int i = start_row; i < end_row; ++i) {
        for (int j = i; j < end_col; ++j) {
            int red_tmp = img.pixels[i][j].red;
            int green_tmp = img.pixels[i][j].green;
            int blue_tmp = img.pixels[i][j].blue;

            img.pixels[i][j].red = img.pixels[j][i].red;
            img.pixels[i][j].green = img.pixels[j][i].green;
            img.pixels[i][j].blue = img.pixels[j][i].blue;

            img.pixels[j][i].red = red_tmp;
            img.pixels[j][i].green = green_tmp;
            img.pixels[j][i].blue = blue_tmp;
        }
    }
}

void transpose_image(image* img, int* height, int* width)
{

    if (*height != *width) {

        int max = get_max(*height, *width);
        //img.pixels = (pixel **)realloc(img.pixels, max * sizeof(pixel *));
        if (*width > *height) {

            (*img).pixels = (pixel**)realloc((*img).pixels, *width * sizeof(pixel*));
            for (int i = *height; i < *width; i++) {
                (*img).pixels[i] = (pixel*)calloc(max, sizeof(pixel));
            }

            transpose_square_matrix((*img), 0, max, max);

            for (int i = 0; i < max; i++)
                (*img).pixels[i] = realloc((*img).pixels[i], *height * sizeof(pixel));
        }         else {
            for (int i = 0; i < *height; i++)
                (*img).pixels[i] = realloc((*img).pixels[i], *height * sizeof(pixel));
            transpose_square_matrix((*img), 0, max, max);

            for (int i = *width; i < *height; i++)
                free((*img).pixels[i]);

            (*img).pixels = realloc((*img).pixels, *width * sizeof(pixel));
        }

        swap(height, width);
        printf("W%d H%d\n", *width, *height);
    }     else
        transpose_square_matrix((*img), 0, *height, *width);
}

void reverse_rows(image img, int start_row, int end_row, int start_col, int end_col)
{
    for (int i = start_row; i < end_row; i++) {
        int low = start_col;
        int high = end_col - 1;
        while (low < high) {
            int red_temp = img.pixels[i][low].red;
            int green_temp = img.pixels[i][low].green;
            int blue_temp = img.pixels[i][low].blue;

            img.pixels[i][low].red = img.pixels[i][high].red;
            img.pixels[i][low].green = img.pixels[i][high].green;
            img.pixels[i][low].blue = img.pixels[i][high].blue;

            img.pixels[i][high].red = red_temp;
            img.pixels[i][high].green = green_temp;
            img.pixels[i][high].blue = blue_temp;

            low++;
            high--;
        }
    }
}

void reverse_cols(image img, int start_row, int end_row, int start_col, int end_col)
{
    for (int i = start_col; i < end_col; i++) {
        int low = start_row;
        int high = end_row - 1;
        while (low < high) {
            int red_temp = img.pixels[low][i].red;
            int green_temp = img.pixels[low][i].green;
            int blue_temp = img.pixels[low][i].blue;

            img.pixels[low][i].red = img.pixels[high][i].red;
            img.pixels[low][i].green = img.pixels[high][i].green;
            img.pixels[low][i].blue = img.pixels[high][i].blue;

            img.pixels[high][i].red = red_temp;
            img.pixels[high][i].green = green_temp;
            img.pixels[high][i].blue = blue_temp;

            low++;
            high--;
        }
    }
}

void free_image(image img, int height)
{
    for (int i = 0; i < height; i++)
        free(img.pixels[i]);
    free(img.pixels);
}

void free_all(netpbm_format img)
{
    if (img.file_in) {
        free_image(img.picture, img.height);
        fclose(img.file_in);
    }
}

void rotate_image_clockwise(netpbm_format* image, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        //transpose
        transpose_image((&(*image).picture), &(*image).height, &(*image).width);
        (*image).curr_selection.p2.x = (*image).width;
        (*image).curr_selection.p2.y = (*image).height;
        // reverse
        reverse_rows((*image).picture, 0, (*image).height, 0, (*image).width);
    }
}

void rotate_image_counter_clockwise(netpbm_format* image, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        //transpose
        transpose_image((&(*image).picture), &(*image).height, &(*image).width);
        (*image).curr_selection.p2.x = (*image).width;
        (*image).curr_selection.p2.y = (*image).height;
        // reverse
        reverse_cols(image->picture, 0, image->height, 0, image->width);
    }
}

void rotate_selection_clockwise(netpbm_format* image, int start_row, int start_col, int end_row, int end_col, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        transpose_square_matrix(image->picture, start_row, end_row, end_col);
        reverse_rows(image->picture, start_row, end_row, start_col, end_col);
    }
}

void rotate_selection_counter_clockwise(netpbm_format* image, int start_row, int start_col, int end_row, int end_col, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        transpose_square_matrix(image->picture, start_row, end_row, end_col);
        reverse_cols(image->picture, start_row, end_row, start_col, end_col);
    }
}

void apply_filter(netpbm_format* img, double kernel[3][3], double fraction)
{
    int delimiter = 3;

    int start_row = get_min(img->curr_selection.p1.y, img->curr_selection.p2.y);
    int end_row = get_max(img->curr_selection.p1.y, img->curr_selection.p2.y);
    int start_col = get_min(img->curr_selection.p1.x, img->curr_selection.p2.x);
    int end_col = get_max(img->curr_selection.p1.x, img->curr_selection.p2.x);
    // copy image
    image temp;
    temp.pixels = (pixel**)malloc(img->height * sizeof(pixel*));
    for (int i = 0; i < img->height; i++)
        temp.pixels[i] = (pixel*)malloc(img->width * sizeof(pixel));

    for (int i = 0; i < img->height; i++)
        for (int j = 0; j < img->width; j++) {
            temp.pixels[i][j].red = img->picture.pixels[i][j].red;
            temp.pixels[i][j].green = img->picture.pixels[i][j].green;
            temp.pixels[i][j].blue = img->picture.pixels[i][j].blue;
        }

    for (int i = start_row; i < end_row; i++) {
        if (i == 0 || i == img->height - 1)
            continue;
        for (int j = start_col; j < end_col; j++) {
            if (j == 0 || j == img->width - 1)
                continue;

            double new_red = 0;
            double new_green = 0;
            double new_blue = 0;

            for (int kernel_row = 0; kernel_row < delimiter; kernel_row++)
                for (int kernel_col = 0; kernel_col < delimiter; kernel_col++) {
                    new_red += (kernel[kernel_row][kernel_col] * temp.pixels[i - 1 + kernel_row][j - 1 + kernel_col].red) / fraction;
                    new_green += (kernel[kernel_row][kernel_col] * temp.pixels[i - 1 + kernel_row][j - 1 + kernel_col].green) / fraction;
                    new_blue += (kernel[kernel_row][kernel_col] * temp.pixels[i - 1 + kernel_row][j - 1 + kernel_col].blue) / fraction;
                }
            // new_red /= fraction;
            // new_blue /= fraction;
            // new_red /= fraction;

            clamp(&new_red, 0, 255);
            clamp(&new_blue, 0, 255);
            clamp(&new_green, 0, 255);
            img->picture.pixels[i][j].red = ceil(new_red);
            img->picture.pixels[i][j].green = ceil(new_green);
            img->picture.pixels[i][j].blue = ceil(new_blue);
        }
    }

    free_image(temp, img->height);
}