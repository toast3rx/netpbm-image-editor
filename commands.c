#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "structs.h"
#include "image_utils.h"
#include "utils.h"

char **read_command(char *command, int *args)
{
	fgets(command, MAX_WORD, stdin);
	command[strlen(command) - 1] = '\0';

	// Count number of words and longest word for memory allocation
	int count_words = 0;
	int max_length = 0;
	int length = 0;

	for (int i = 0; i < (int)strlen(command); i++) {
		if (command[i] != ' ')
			length++;
		else {
			count_words++;
			if (max_length < length)
				max_length = length;
			length = 0;
		}
	}
	count_words++;
	if (length > max_length)
		max_length = length;

	char **words = (char **)malloc(count_words * sizeof(char *));
	if (!words) {
		printf("Error!\n");
		return NULL;
	}
	for (int i = 0; i < count_words; i++) {
		words[i] = (char *)malloc((max_length + 1) * sizeof(char));
		if (!words[i]) {
			printf("Error\n");
			return NULL;
		}
	}

	split_string(command, words);

	*args = count_words;
	return words;
}

void load_file(char *file_name, netpbm_format *netpbm_img)
{

	if (netpbm_img->file_in != NULL) {
		free_all(*netpbm_img);
	}

	netpbm_img->file_in = fopen(file_name, "r");
	if (!netpbm_img->file_in) {
		printf("Failed to load %s\n", file_name);
		return;
	}

	check_comments(netpbm_img->file_in);

	fgets(netpbm_img->magic_number, 3, netpbm_img->file_in);
	netpbm_img->magic_number[2] = '\0'; // ?????
	if (netpbm_img->magic_number[0] != 'P') {
		printf("Wrong Image format\n");
		return;
	}

	getc(netpbm_img->file_in); // \n
	check_comments(netpbm_img->file_in);

	fscanf(netpbm_img->file_in, "%d", &(netpbm_img->width));
	fscanf(netpbm_img->file_in, "%d", &(netpbm_img->height));

	getc(netpbm_img->file_in);
	check_comments(netpbm_img->file_in);

	fscanf(netpbm_img->file_in, "%d", &(netpbm_img->max_value));

	if (netpbm_img->magic_number[1] == '3' || netpbm_img->magic_number[1] == '6') {
		netpbm_img->type = PPM;
	} else if (netpbm_img->magic_number[1] == '2' || netpbm_img->magic_number[1] == '5') {
		netpbm_img->type = PGM;
	}

	// Alloc
	netpbm_img->picture.pixels = (pixel **)malloc(netpbm_img->height * sizeof(pixel *));
	if (!netpbm_img->picture.pixels) {
		printf("Error allocation!\n");
		return;
	}
	for (int i = 0; i < netpbm_img->height; i++) {
		netpbm_img->picture.pixels[i] = (pixel *)malloc(netpbm_img->width * sizeof(pixel));
		if (!netpbm_img->picture.pixels[i]) {
			printf("Error allocation!\n");
			return;
		}
	}

	getc(netpbm_img->file_in);
	check_comments(netpbm_img->file_in);

	if (netpbm_img->type == PPM) {
		if (netpbm_img->magic_number[1] == '3') {
			for (int i = 0; i < netpbm_img->height; i++) {
				for (int j = 0; j < netpbm_img->width; j++) {
					fscanf(netpbm_img->file_in, "%d", &(netpbm_img->picture.pixels[i][j].red));
					fscanf(netpbm_img->file_in, "%d", &(netpbm_img->picture.pixels[i][j].green));
					fscanf(netpbm_img->file_in, "%d", &(netpbm_img->picture.pixels[i][j].blue));
				}
			}
		} else {
			for (int i = 0; i < netpbm_img->height; i++)
				for (int j = 0; j < netpbm_img->width; j++) {
					fread(&(netpbm_img->picture.pixels[i][j].red), sizeof(int), 1, netpbm_img->file_in);
					fread(&(netpbm_img->picture.pixels[i][j].green), sizeof(int), 1, netpbm_img->file_in);
					fread(&(netpbm_img->picture.pixels[i][j].blue), sizeof(int), 1, netpbm_img->file_in);
				}
		}
	} else if (netpbm_img->type == PGM) {
		if (netpbm_img->magic_number[1] == '2')
			for (int i = 0; i < netpbm_img->height; i++)
				for (int j = 0; j < netpbm_img->width; j++) {
					int value;
					fscanf(netpbm_img->file_in, "%d", &value);
					netpbm_img->picture.pixels[i][j].red = value;
					netpbm_img->picture.pixels[i][j].green = value;
					netpbm_img->picture.pixels[i][j].blue = value;
				} else
					for (int i = 0; i < netpbm_img->height; i++)
						for (int j = 0; j < netpbm_img->width; j++) {
							int value;
							fread(&value, sizeof(int), 1, netpbm_img->file_in);
							netpbm_img->picture.pixels[i][j].red = value;
							netpbm_img->picture.pixels[i][j].green = value;
							netpbm_img->picture.pixels[i][j].blue = value;
						}
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
	// int x1 = args[1][0] - '0';
	// int y1 = args[2][0] - '0';
	// int x2 = args[3][0] - '0';
	// int y2 = args[4][0] - '0';
	int x1 = string_to_int(args[1]);
	int y1 = string_to_int(args[2]);
	int x2 = string_to_int(args[3]);
	int y2 = string_to_int(args[4]);


	if (x1 < 0 || x1 > image->width || x2 < 0 || x2 > image->width ||
		y1 < 0 || y1 > image->height || y2 < 0 || y2 > image->height) {
		printf("Invalid coordinates!\n");
		return;
	}
	if (!image->file_in) {
		printf("No image loaded!\n");
		return;
	}
	image->curr_selection.p1.x = x1;
	image->curr_selection.p1.y = y1;
	image->curr_selection.p2.x = x2;
	image->curr_selection.p2.y = y2;
	printf("Selected %d %d %d %d\n", image->curr_selection.p1.x, image->curr_selection.p1.y, image->curr_selection.p2.x, image->curr_selection.p2.y);
}

void select_all_command(netpbm_format img)
{

	if (!img.file_in) {
		printf("No image loaded!\n");
		return;
	}
	img.curr_selection.p1.x = 0;
	img.curr_selection.p1.y = 0;
	img.curr_selection.p2.x = img.width;
	img.curr_selection.p2.y = img.height;

	printf("Selected ALL!\n");
}

void rotate_command(netpbm_format *image, char *angle)
{

	if (image->file_in == NULL) {
		printf("No image loaded\n");
		return;
	}

	char sign = angle[0];
	int angle_converted = string_to_int(angle + 1);

	if ((angle_converted % 90 != 0 || angle_converted / 90 > 4) || (sign != '-' && sign != '+'))
		printf("Unsupported rotation angle\n");

	int start_row = get_min((*image).curr_selection.p1.y, (*image).curr_selection.p2.y);
	int end_row = get_max((*image).curr_selection.p1.y, (*image).curr_selection.p2.y);

	int start_col = get_min((*image).curr_selection.p1.x, (*image).curr_selection.p2.x);
	int end_col = get_max((*image).curr_selection.p1.x, (*image).curr_selection.p2.x);

	if (end_row - start_row == (*image).height && end_col - start_col == (*image).width) {
		// rotate all image
		if (sign == '+') {
			rotate_image_clockwise(image, angle_converted);
		}
		if (sign == '-') {
			rotate_image_counter_clockwise(image, angle_converted);
		}
	} else if (end_row - start_row == end_col - start_col) {
		// rotate selection
		if (sign == '+') {
			rotate_selection_clockwise(image, start_row, start_col, end_row, end_col, angle_converted);
		} else if (sign == '-') {
			rotate_selection_counter_clockwise(image, start_row, start_col, end_row, end_col, angle_converted);
		}
	} else {
		printf("The selection must be square\n");
		return;
	}

	printf("Rotated %c%d\n", sign, angle_converted);

}

void crop_command(netpbm_format *img)
{

	if (img->file_in == NULL) {
		printf("Error allocation!\n");
		return;
	}

	image selected_img;
	int height_start = get_min(img->curr_selection.p1.y, img->curr_selection.p2.y);
	int height_end = get_max(img->curr_selection.p1.y, img->curr_selection.p2.y);
	int width_start = get_min(img->curr_selection.p1.x, img->curr_selection.p2.x);
	int width_end = get_max(img->curr_selection.p1.x, img->curr_selection.p2.x);

	int selection_height = height_end - height_start;
	int selection_width = width_end - width_start;

	selected_img.pixels = (pixel **)malloc(selection_height * sizeof(pixel *));
	if (!selected_img.pixels) {
		printf("Error allocation!\n");
		return;
	}
	for (int i = 0; i < selection_height; i++) {
		selected_img.pixels[i] = (pixel *)calloc(selection_width, sizeof(pixel));
		if (!selected_img.pixels[i]) {
			printf("Error allocation!\n");
			return;
		}
	}

	for (int i = 0; i < selection_height; i++)
		for (int j = 0; j < selection_width; j++) {
			selected_img.pixels[i][j].red = img->picture.pixels[i + height_start][j + width_start].red;
			selected_img.pixels[i][j].green = img->picture.pixels[i + height_start][j + width_start].green;
			selected_img.pixels[i][j].blue = img->picture.pixels[i + height_start][j + width_start].blue;
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

	if (img->file_in == NULL) {
		printf("No image loaded\n");
		return;
	}

	if (img->type == PGM) {
		printf("Easy, Charlie Chapin\n");
		return;
	}
	if (strcmp(filter, EDGE_FILTER) == 0) {
		double edge_kernel[3][3] = {
			{-1.0, -1.0, -1.0},
			{-1.0, 8.0, -1.0},
			{-1.0, -1.0, -1.0}
		};
		apply_filter(img, edge_kernel, 1);
	} else if (strcmp(filter, SHARPEN_FILTER) == 0) {
		double sharpen_kernel[3][3] = {
			{0.0, -1.0, 0.0},
			{-1.0, 5.0, -1.0},
			{0.0, -1.0, 0.0}
		};
		apply_filter(img, sharpen_kernel, 1);
	} else if (strcmp(filter, BLUR_FILTER) == 0) {
		double blur_kernel[3][3] = {
			{1.0, 1.0, 1.0},
			{1.0, 1.0, 1.0},
			{1.0, 1.0, 1.0}
		};
		apply_filter(img, blur_kernel, 9.0);
	} else if (strcmp(filter, GAUSSIAN_BLUR_FILTER) == 0) {
		double gaussain_blur_kernel[3][3] = {
			{0.0625, 0.125, 0.0625},
			{0.125, 0.25 , 0.125},
			{0.0625, 0.125, 0.0625}
		};
		apply_filter(img, gaussain_blur_kernel, 1);
	} else {
		printf("APPLY paramter invalid\n");
		return;
	}

	printf("APPLY %s done\n", filter);
}

void save_command(netpbm_format img, char *file_name, char *is_ascii)
{
	///MAGIC NUMBER CHANGE
	int ascii = 0;
	if (strcmp(is_ascii, "ascii") == 0)
		ascii = 1;

	FILE *fin;
	fin = fopen(file_name, "wt");
	if (!fin) {
		printf("Error opening file\n");
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
				if (img.type == PPM)
					fprintf(fin, "%d %d %d ", img.picture.pixels[i][j].red, img.picture.pixels[i][j].green, img.picture.pixels[i][j].blue);
				else
					fprintf(fin, "%d ", img.picture.pixels[i][j].red);
				//printf("%d ", img.picture.pixels[i][j].red);
			}
			fprintf(fin, "\n");
			//printf("\n");
		}
	} else {
		fclose(fin);
		fin = fopen(file_name, "ab");
		if (!fin) {
			printf("Error opening file\n");
			return;
		}
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				if (img.type == PPM) {
					fwrite(&(img.picture.pixels[i][j].red), sizeof(char), 1, fin);
					fwrite(&(img.picture.pixels[i][j].green), sizeof(char), 1, fin);
					fwrite(&(img.picture.pixels[i][j].blue), sizeof(char), 1, fin);
				} else
					fwrite(&(img.picture.pixels[i][j].red), sizeof(char), 1, fin);
			}
			//fwrite("\n", sizeof(char), 1, fin);
		}
	}

	fclose(fin);
	printf("Saved %s\n", file_name);
}
