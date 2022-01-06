#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "constants.h"
#include "structs.h"
#include "utils.h"
#include "image_utils.h"
#include "commands.h"

/**
 * @brief
 * Check if inserted command is the LOAD command
 * @param command input from stdin
 * @return 1 for true, 0 for false
 */
int is_load_command(char *command)
{
	if (strcmp(command, LOAD) == 0)
		return 1;
	else
		return 0;
}

/**
 * @brief Check for select command
 *
 * @param command given by the user
 * @return int true or false
 */
int is_select_command(char *command)
{
	if (strcmp(command, SELECT) == 0)
		return 1;
	else
		return 0;
}

/**
 * @brief Check for select all command
 *
 * @param command given by the user
 * @param arg second argument
 * @return int true or false
 */
int is_select_all_command(char *command, char *arg)
{
	if (is_select_command(command)) {
		if (strcmp(arg, SELECT_ALL) == 0)
			return 1;
	}

	return 0;
}

int is_rotate_command(char **args, int length)
{
	if (strcmp(args[0], ROTATE) != 0)
		return 0;
	if (length == 2 && args[1])
		return 1;

	printf("Unsupported rotation angle!\n");
	return 0;
}

void print_image(netpbm_format img)
{
	printf("%s\n%d %d\n%d\n", img.magic_number, img.width, img.height, img.max_value);
	for (int i = 0; i < img.height; i++) {
		for (int j = 0; j < img.width; j++)
			printf("%d %d %d       ", img.picture.pixels[i][j].red, img.picture.pixels[i][j].green, img.picture.pixels[i][j].blue);

		printf("\n");
	}
}

int main(void)
{
	char *command = (char *)malloc((MAX_WORD + 1) * sizeof(char));
	if (!command) {
		printf("Error!\n");
		return -1;
	}
	char **args;
	int command_length = 0;

	netpbm_format img;
	img.file_in = NULL;

	while (1) {
		if (command_length > 0)
			free_char_2D_array(args, command_length);
		args = read_command(command, &command_length);
		if (strcmp(command, EXIT) == 0) {
			break;
		} else if (is_load_command(args[0])) {
			if (args[1])
				load_file(args[1], &img);
			else
				printf("Please enter a file name!\n");
		} else if (is_select_command(args[0])) {
			if (command_length == 2 && is_select_all_command(args[0], args[1]))
				printf("SELECT ALL!\n");
			else
				if (command_length != 5)
					printf("Invalid number of coordinates!\n");
				else
					select_command(args, &img);
		} else if (is_rotate_command(args, command_length)) {
			rotate_command(&img, args[1]);
		} else if (strcmp(command, CROP) == 0) {
			crop_command(&img);
		} else if (strcmp(args[0], APPLY) == 0) {
			apply_command(&img, args[1]);
		} else if (strcmp(args[0], SAVE) == 0) {
			char *is_ascii;
			if (command_length == 3)
				is_ascii = args[2];
			else
				is_ascii = " ";
			save_command(img, args[1], is_ascii);
		} else if (strcmp(command, "PRINT") == 0) {
			print_image(img);
		} else {
			printf("Unknow command!\n");
		}
	}

	free(command);
	free_char_2D_array(args, command_length);

	free_all(img);
	return 0;
}

