#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "structs.h"
#include "image_utils.h"
#include "utils.h"
#include "exceptions.h"

char **read_command(char *command, int *args)
{
	fgets(command, MAX_WORD, stdin);
	if (strcmp(command, EXIT))
		command[strlen(command) - 1] = '\0';

	// Count number of words and longest word for memory allocation
	int count_words = 0;
	int max_length = 0;
	int length = 0;

	for (int i = 0; i < (int)strlen(command); i++) {
		if (command[i] != ' ') {
			length++;
		} else {
			count_words++;
			if (max_length < length)
				max_length = length;
			length = 0;
		}
	}
	count_words++;
	if (length > max_length)
		max_length = length;

	char **words = (char **)calloc(count_words, sizeof(char *));
	if (!words) {
		memory_allocation_exception();
		return NULL;
	}
	for (int i = 0; i < count_words; i++) {
		words[i] = (char *)calloc((max_length + 1), sizeof(char));
		if (!words[i]) {
			memory_allocation_exception();
			return NULL;
		}
	}

	split_string(command, words);

	*args = count_words;
	return words;
}

void load_file(char *file_name, netpbm_format *netpbm_img)
{
	if (netpbm_img->file_in)
		free_all(*netpbm_img);

	netpbm_img->file_in = fopen(file_name, "r");
	if (!netpbm_img->file_in) {
		open_file_exception(file_name);
		return;
	}

	check_comments(netpbm_img->file_in);

	fgets(netpbm_img->magic_number, 3, netpbm_img->file_in);
	netpbm_img->magic_number[2] = '\0';
	if (netpbm_img->magic_number[0] != 'P') {
		printf("Wrong Image format\n");
		return;
	}

	getc(netpbm_img->file_in);
	check_comments(netpbm_img->file_in);

	fscanf(netpbm_img->file_in, "%d", &netpbm_img->width);
	fscanf(netpbm_img->file_in, "%d", &netpbm_img->height);

	getc(netpbm_img->file_in);
	check_comments(netpbm_img->file_in);

	fscanf(netpbm_img->file_in, "%d", &netpbm_img->max_value);

	if (netpbm_img->magic_number[1] == '3' ||
		netpbm_img->magic_number[1] == '6')
		netpbm_img->type = PPM;
	else if (netpbm_img->magic_number[1] == '2' ||
			 netpbm_img->magic_number[1] == '5')
		netpbm_img->type = PGM;

	netpbm_img->picture.pixels = (pixel **)calloc(netpbm_img->height,
									sizeof(pixel *));
	if (!netpbm_img->picture.pixels) {
		memory_allocation_exception();
		return;
	}

	for (int i = 0; i < netpbm_img->height; i++) {
		netpbm_img->picture.pixels[i] = (pixel *)calloc(netpbm_img->width,
											sizeof(pixel));
		if (!netpbm_img->picture.pixels[i]) {
			memory_allocation_exception();
			return;
		}
	}

	getc(netpbm_img->file_in);
	check_comments(netpbm_img->file_in);

	if (netpbm_img->type == PPM) {
		if (netpbm_img->magic_number[1] == '3')
			read_pixels_ppm_ascii(netpbm_img);
		else
			read_pixels_ppm_binary(netpbm_img);
	} else if (netpbm_img->type == PGM) {
		if (netpbm_img->magic_number[1] == '2')
			read_pixels_pgm_ascii(netpbm_img);
		else
			read_pixels_pgm_binary(netpbm_img);
	}
	// Save current selection as entire image
	netpbm_img->curr_selection.p1.x = 0;
	netpbm_img->curr_selection.p1.y = 0;
	netpbm_img->curr_selection.p2.x = netpbm_img->width;
	netpbm_img->curr_selection.p2.y = netpbm_img->height;

	printf("Loaded %s\n", file_name);
}

void select_command(char **args, netpbm_format *image)
{
	if (!image->file_in) {
		no_file_load_exception();
		return;
	}

	if (args[1][0] == '-' || args[2][0] == '-' ||
		args[3][0] == '-' || args[4][0] == '-') {
		invalid_coord_exception();
		return;
	}

	if (!(is_numeric(args[1]) && is_numeric(args[2]) &&
		  is_numeric(args[3]) && is_numeric(args[4]))) {
		invalid_command_exception();
		return;
	}
	int x1 = string_to_int(args[1]);
	int y1 = string_to_int(args[2]);
	int x2 = string_to_int(args[3]);
	int y2 = string_to_int(args[4]);

	if (x1 > x2)
		swap(&x1, &x2);
	if (y1 > y2)
		swap(&y1, &y2);

	if (x1 == x2 || y1 == y2) {
		invalid_coord_exception();
		return;
	}

	if (x1 < 0 || x1 > image->width || x2 < 0 || x2 > image->width ||
		y1 < 0 || y1 > image->height || y2 < 0 || y2 > image->height) {
		invalid_coord_exception();
		return;
	}
	image->curr_selection.p1.x = x1;
	image->curr_selection.p1.y = y1;
	image->curr_selection.p2.x = x2;
	image->curr_selection.p2.y = y2;
	printf("Selected %d %d %d %d\n",
		   image->curr_selection.p1.x,
		   image->curr_selection.p1.y,
		   image->curr_selection.p2.x,
		   image->curr_selection.p2.y);
}

void select_all_command(netpbm_format *img)
{
	if (!img->file_in) {
		no_file_load_exception();
		return;
	}
	img->curr_selection.p1.x = 0;
	img->curr_selection.p1.y = 0;
	img->curr_selection.p2.x = img->width;
	img->curr_selection.p2.y = img->height;

	printf("Selected ALL\n");
}

void rotate_command(netpbm_format *image, char *angle)
{
	if (!image->file_in) {
		no_file_load_exception();
		return;
	}

	char sign;
	int angle_converted;
	if (angle[0] == '-') {
		sign = angle[0];
		angle_converted = string_to_int(angle + 1);
	} else {
		sign = '+';
		angle_converted = string_to_int(angle);
	}

	if ((angle_converted % 90 != 0 || angle_converted / 90 > 4)) {
		invalid_angle_exception();
		return;
	}

	if (angle_converted == 0) {
		printf("Rotated %d\n", angle_converted);
		return;
	}

	int start_row = get_min((*image).curr_selection.p1.y,
							(*image).curr_selection.p2.y);
	int end_row = get_max((*image).curr_selection.p1.y,
						  (*image).curr_selection.p2.y);

	int start_col = get_min((*image).curr_selection.p1.x,
							(*image).curr_selection.p2.x);
	int end_col = get_max((*image).curr_selection.p1.x,
							(*image).curr_selection.p2.x);

	if (end_row - start_row == (*image).height &&
		end_col - start_col == (*image).width) {
		if (sign == '+')
			rotate_image_clockwise(image, angle_converted);
		else if (sign == '-')
			rotate_image_counter_clockwise(image, angle_converted);
	} else if (end_row - start_row == end_col - start_col) {
		if (sign == '+')
			rotate_selection_clockwise(image,
									   start_row,
									   start_col,
									   end_row,
									   end_col,
									   angle_converted);
		else if (sign == '-')
			rotate_selection_counter_clockwise(image,
											   start_row,
											   start_col,
											   end_row, end_col,
											   angle_converted);
	} else {
		square_selection_exception();
		return;
	}

	if (sign == '-')
		printf("Rotated %c%d\n", sign, angle_converted);
	else
		printf("Rotated %d\n", angle_converted);
}

void crop_command(netpbm_format *img)
{
	if (!img->file_in) {
		no_file_load_exception();
		return;
	}

	image selected_img;
	int height_start = get_min(img->curr_selection.p1.y,
								img->curr_selection.p2.y);
	int height_end = get_max(img->curr_selection.p1.y,
								img->curr_selection.p2.y);
	int width_start = get_min(img->curr_selection.p1.x,
								img->curr_selection.p2.x);
	int width_end = get_max(img->curr_selection.p1.x,
								img->curr_selection.p2.x);

	int selection_height = height_end - height_start;
	int selection_width = width_end - width_start;

	selected_img.pixels = (pixel **)calloc(selection_height, sizeof(pixel *));
	if (!selected_img.pixels) {
		memory_allocation_exception();
		return;
	}
	for (int i = 0; i < selection_height; i++) {
		selected_img.pixels[i] = (pixel *)calloc(selection_width,
													sizeof(pixel));
		if (!selected_img.pixels[i]) {
			memory_allocation_exception();
			return;
		}
	}

	for (int i = 0; i < selection_height; i++)
		for (int j = 0; j < selection_width; j++) {
			selected_img.pixels[i][j].red = img->picture.pixels[i +
												height_start][j +
												width_start].red;
			selected_img.pixels[i][j].green = img->picture.pixels[i +
												height_start][j +
												width_start].green;
			selected_img.pixels[i][j].blue = img->picture.pixels[i +
												height_start][j +
												width_start].blue;
		}

	free_image(img->picture, img->height);

	img->width = selection_width;
	img->height = selection_height;

	img->curr_selection.p1.x = 0;
	img->curr_selection.p1.y = 0;
	img->curr_selection.p2.x = selection_width;
	img->curr_selection.p2.y = selection_height;

	img->picture = selected_img;

	printf("Image cropped\n");
}

void apply_command(netpbm_format *img, char *filter)
{
	if (img->type == PGM) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	if (strcmp(filter, EDGE_FILTER) == 0) {
		double edge_kernel[3][3] = {
									{-1, -1, -1},
									{-1, 8, -1},
									{-1, -1, -1}
									};
		move_decimals(edge_kernel, 3, DECIMAL_PLACES);
		apply_filter(img, edge_kernel);
	} else if (strcmp(filter, SHARPEN_FILTER) == 0) {
		double sharpen_kernel[3][3] = {
										{0, -1, 0},
										{-1, 5, -1},
										{0, -1, 0}
										};
		move_decimals(sharpen_kernel, 3, DECIMAL_PLACES);
		apply_filter(img, sharpen_kernel);
	} else if (strcmp(filter, BLUR_FILTER) == 0) {
		double val = 0.1111;
		double blur_kernel[3][3] = {
									{val, val, val},
									{val, val, val},
									{val, val, val}
									};
		move_decimals(blur_kernel, 3, DECIMAL_PLACES);
		apply_filter(img, blur_kernel);
	} else if (strcmp(filter, GAUSSIAN_BLUR_FILTER) == 0) {
		double gaussain_blur_kernel[3][3] = {
											{0.0625, 0.125, 0.0625},
											{0.125, 0.25, 0.125},
											{0.0625, 0.125, 0.0625}
											};
		move_decimals(gaussain_blur_kernel, 3, DECIMAL_PLACES);
		apply_filter(img, gaussain_blur_kernel);
	} else {
		apply_parameter_exception();
		return;
	}

	printf("APPLY %s done\n", filter);
}

void save_command(netpbm_format img, char *file_name, char *is_ascii)
{
	if (!img.file_in) {
		no_file_load_exception();
		return;
	}
	int ascii = 0;
	if (strcmp(is_ascii, "ascii") == 0)
		ascii = 1;

	FILE *fin;
	fin = fopen(file_name, "wt");
	if (!fin) {
		open_file_exception(file_name);
		return;
	}

	char ppm_no[2] = { '6', '3' };
	char pgm_no[2] = { '5', '2' };
	char magic_number;
	if (img.type == PPM)
		magic_number = ppm_no[ascii];
	else if (img.type == PGM)
		magic_number = pgm_no[ascii];
	fprintf(fin, "%c%c\n", 'P', magic_number);
	fprintf(fin, "%d %d\n", img.width, img.height);
	fprintf(fin, "%d\n", img.max_value);

	if (ascii) {
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				if (img.type == PPM) {
					unsigned char r = round(1.0 *
											img.picture.pixels[i][j].red /
											PRECISION);
					unsigned char g = round(1.0 *
											img.picture.pixels[i][j].green /
											PRECISION);
					unsigned char b = round(1.0 *
											img.picture.pixels[i][j].blue /
											PRECISION);
					fprintf(fin, "%hhu %hhu %hhu ", r, g, b);
				} else {
					fprintf(fin, "%hhu ", img.picture.pixels[i][j].red);
				}
			}
			fprintf(fin, "\n");
		}
	} else {
		fclose(fin);
		fin = fopen(file_name, "ab");
		if (!fin) {
			open_file_exception(file_name);
			return;
		}
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				if (img.type == PPM) {
					unsigned char r = round(1.0 *
											img.picture.pixels[i][j].red /
											PRECISION);
					unsigned char g = round(1.0 *
											img.picture.pixels[i][j].green /
											PRECISION);
					unsigned char b = round(1.0 *
											img.picture.pixels[i][j].blue /
											PRECISION);
					fwrite(&r, sizeof(char), 1, fin);
					fwrite(&g, sizeof(char), 1, fin);
					fwrite(&b, sizeof(char), 1, fin);
				} else {
					fwrite(&img.picture.pixels[i][j].red,
						   sizeof(char),
						   1,
						   fin);
				}
			}
		}
	}
	fclose(fin);
	printf("Saved %s\n", file_name);
}
