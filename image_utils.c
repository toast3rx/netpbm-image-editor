#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <string.h>
#include "image_utils.h"
#include "utils.h"
#include "constants.h"
#include "exceptions.h"

void transpose_square_matrix(image img,
							 int start_row,
							 int end_row,
							 int start_col,
							 int end_col)
{
	pixel **temp_img = (pixel **)calloc((end_row - start_row),
										 sizeof(pixel *));
	if (!temp_img) {
		memory_allocation_exception();
		return;
	}

	for (int i = 0; i < end_row - start_row; i++) {
		temp_img[i] = (pixel *)calloc((end_col - start_col), sizeof(pixel));
		if (!temp_img[i]) {
			memory_allocation_exception();
			return;
		}
	}

	for (int i = 0; i < end_row - start_row; i++)
		for (int j = 0; j < end_col - start_col; j++) {
			temp_img[i][j].red =
				img.pixels[i + start_row][j + start_col].red;
			temp_img[i][j].green =
				img.pixels[i + start_row][j + start_col].green;
			temp_img[i][j].blue =
				img.pixels[i + start_row][j + start_col].blue;
		}

	for (int i = 0; i < end_row - start_row; i++)
		for (int j = 0; j < end_col - start_col; j++) {
			img.pixels[i + start_row][j + start_col].red =
				temp_img[j][i].red;
			img.pixels[i + start_row][j + start_col].green =
				temp_img[j][i].green;
			img.pixels[i + start_row][j + start_col].blue =
				temp_img[j][i].blue;
		}

	for (int i = 0; i < end_row - start_row; i++)
		free(temp_img[i]);
	free(temp_img);
}

void transpose_image(image *img, int *height, int *width)
{
	if (*height != *width) {
		int max = get_max(*height, *width);
		if (*width > *height) {
			pixel **realloc_tmp = (pixel **)realloc((*img).pixels,
							 *width * sizeof(pixel *));
			if (!realloc_tmp) {
				memory_allocation_exception();
				return;
			}
			img->pixels = realloc_tmp;
			for (int i = *height; i < *width; i++) {
				img->pixels[i] = (pixel *)calloc(max, sizeof(pixel));
				if (!(*img).pixels[i]) {
					memory_allocation_exception();
					return;
				}
			}
			transpose_square_matrix((*img), 0, max, 0, max);

			for (int i = 0; i < max; i++) {
				pixel *tmp = realloc((*img).pixels[i],
											*height * sizeof(pixel));
				if (!tmp) {
					memory_allocation_exception();
					return;
				}
				img->pixels[i] = tmp;
			}
		} else {
			for (int i = 0; i < *height; i++) {
				pixel *tmp = realloc((*img).pixels[i],
											*height * sizeof(pixel));
				if (!tmp) {
					memory_allocation_exception();
					return;
				}
				img->pixels[i] = tmp;
			}

			transpose_square_matrix((*img), 0, max, 0, max);

			for (int i = *width; i < *height; i++)
				free((*img).pixels[i]);

			pixel **r_tmp = realloc((*img).pixels, *width * sizeof(pixel));
			if (!r_tmp) {
				memory_allocation_exception();
				return;
			}
			img->pixels = r_tmp;
		}

		swap(height, width);
	} else {
		transpose_square_matrix((*img), 0, *height, 0, *width);
	}
}

void reverse_rows(image img,
				  int start_row,
				  int end_row,
				  int start_col,
				  int end_col)
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

void reverse_cols(image img,
				  int start_row,
				  int end_row,
				  int start_col,
				  int end_col)
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

void rotate_image_clockwise(netpbm_format *image, int angle)
{
	for (int k = 0; k < (angle / 90); k++) {
		//transpose
		transpose_image((&image->picture), &image->height, &image->width);
		(*image).curr_selection.p2.x = image->width;
		(*image).curr_selection.p2.y = image->height;
		// reverse
		reverse_rows((*image).picture, 0, (*image).height, 0, (*image).width);
	}
}

void rotate_image_counter_clockwise(netpbm_format *image, int angle)
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

void rotate_selection_clockwise(netpbm_format *image,
								int start_row,
								int start_col,
								int end_row,
								int end_col,
								int angle)
{
	if (angle == 360)
		return;
	for (int k = 0; k < (angle / 90); k++) {
		transpose_square_matrix(image->picture,
								start_row,
								end_row,
								start_col,
								end_col);
		reverse_rows(image->picture, start_row, end_row, start_col, end_col);
	}
}

void rotate_selection_counter_clockwise(netpbm_format *image,
										int start_row,
										int start_col,
										int end_row,
										int end_col,
										int angle)
{
	if (angle == -360)
		return;
	for (int k = 0; k < (angle / 90); k++) {
		transpose_square_matrix(image->picture,
								start_row,
								end_row,
								start_col,
								end_col);
		reverse_cols(image->picture, start_row, end_row, start_col, end_col);
	}
}

void apply_filter(netpbm_format *img, double kernel[3][3])
{
	int del = 3;

	int start_row = get_min(img->curr_selection.p1.y,
							img->curr_selection.p2.y);
	int end_row = get_max(img->curr_selection.p1.y,
						  img->curr_selection.p2.y);
	int start_col = get_min(img->curr_selection.p1.x,
							img->curr_selection.p2.x);
	int end_col = get_max(img->curr_selection.p1.x,
						  img->curr_selection.p2.x);
	// copy image
	image temp;
	temp.pixels = (pixel **)malloc(img->height * sizeof(pixel *));
	if (!temp.pixels) {
		memory_allocation_exception();
		return;
	}

	for (int i = 0; i < img->height; i++) {
		temp.pixels[i] = (pixel *)calloc(img->width, sizeof(pixel));
		if (!temp.pixels[i]) {
			memory_allocation_exception();
			return;
		}
	}

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

			for (int kernel_row = 0; kernel_row < del; kernel_row++)
				for (int kernel_col = 0; kernel_col < del; kernel_col++) {
					int temp_row = i - 1 + kernel_row;
					int temp_col = j - 1 + kernel_col;
					new_red += (kernel[kernel_row][kernel_col] *
								temp.pixels[temp_row][temp_col].red);
					new_green += (kernel[kernel_row][kernel_col] *
								  temp.pixels[temp_row][temp_col].green);
					new_blue += (kernel[kernel_row][kernel_col] *
								temp.pixels[temp_row][temp_col].blue);
				}

			new_red /= 10000;
			new_green /= 10000;
			new_blue /= 10000;
			clamp(&new_red, 0, 255 * PRECISION);
			clamp(&new_green, 0, 255 * PRECISION);
			clamp(&new_blue, 0, 255 * PRECISION);

			img->picture.pixels[i][j].red = new_red;
			img->picture.pixels[i][j].green = new_green;
			img->picture.pixels[i][j].blue = new_blue;
		}
	}
	free_image(temp, img->height);
}

int is_numeric(char *str)
{
	for (int i = 0; i < (int)strlen(str); i++)
		if (!(str[i] <= '9' && str[i] >= '0'))
			return 0;
	return 1;
}

void read_pixels_ppm_ascii(netpbm_format *netpbm_img)
{
	for (int i = 0; i < netpbm_img->height; i++) {
		for (int j = 0; j < netpbm_img->width; j++) {
			fscanf(netpbm_img->file_in, "%d",
				   &netpbm_img->picture.pixels[i][j].red);
			fscanf(netpbm_img->file_in, "%d",
				   &netpbm_img->picture.pixels[i][j].green);
			fscanf(netpbm_img->file_in, "%d",
				   &netpbm_img->picture.pixels[i][j].blue);
			netpbm_img->picture.pixels[i][j].red *= PRECISION;
			netpbm_img->picture.pixels[i][j].green *= PRECISION;
			netpbm_img->picture.pixels[i][j].blue *= PRECISION;
		}
	}
}

void read_pixels_ppm_binary(netpbm_format *netpbm_img)
{
	for (int i = 0; i < netpbm_img->height; i++)
		for (int j = 0; j < netpbm_img->width; j++) {
			fread(&netpbm_img->picture.pixels[i][j].red,
				  sizeof(char),
				  1,
				  netpbm_img->file_in);
			fread(&netpbm_img->picture.pixels[i][j].green,
				  sizeof(char),
				  1,
				  netpbm_img->file_in);
			fread(&netpbm_img->picture.pixels[i][j].blue,
				  sizeof(char),
				  1,
				  netpbm_img->file_in);
			netpbm_img->picture.pixels[i][j].red *= PRECISION;
			netpbm_img->picture.pixels[i][j].green *= PRECISION;
			netpbm_img->picture.pixels[i][j].blue *= PRECISION;
		}
}

void read_pixels_pgm_ascii(netpbm_format *netpbm_img)
{
	for (int i = 0; i < netpbm_img->height; i++)
		for (int j = 0; j < netpbm_img->width; j++) {
			fscanf(netpbm_img->file_in, "%d",
				   &netpbm_img->picture.pixels[i][j].red);
			netpbm_img->picture.pixels[i][j].green =
				netpbm_img->picture.pixels[i][j].red;
			netpbm_img->picture.pixels[i][j].blue =
				netpbm_img->picture.pixels[i][j].red;
		}
}

void read_pixels_pgm_binary(netpbm_format *netpbm_img)
{
	for (int i = 0; i < netpbm_img->height; i++)
		for (int j = 0; j < netpbm_img->width; j++) {
			fread(&netpbm_img->picture.pixels[i][j].red,
				  sizeof(char),
				  1,
				  netpbm_img->file_in);
			netpbm_img->picture.pixels[i][j].green =
				netpbm_img->picture.pixels[i][j].red;
			netpbm_img->picture.pixels[i][j].blue =
				netpbm_img->picture.pixels[i][j].red;
		}
}
