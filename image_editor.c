#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "constants.h"
#include "structs.h"
#include "utils.h"
#include "image_utils.h"
#include "commands.h"
#include "command_check.h"

int main(void)
{
	char *command = (char *)calloc((MAX_WORD + 1), sizeof(char));
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
		if (is_exit_command(args[0], img)) {
			break;
		} else if (is_load_command(args[0])) {
			if (args[1])
				load_file(args[1], &img);
			else
				printf("Please enter a file name!\n");
		} else if (is_select_command(args[0])) {
			if (command_length >= 2 && is_select_all_command(args[0], args[1]))
				select_all_command(&img);
			else
				if (command_length < 5)
					invalid_command_exception();
				else
					select_command(args, &img);
		} else if (is_rotate_command(args, command_length)) {
			rotate_command(&img, args[1]);
		} else if (is_crop_command(command)) {
			crop_command(&img);
		} else if (is_apply_filter_command(args[0])) {
			if (!img.file_in)
				no_file_load_exception();
			else if (command_length != 2)
				invalid_command_exception();
			else
				apply_command(&img, args[1]);
		} else if (is_save_command(args[0])) {
			char *is_ascii;
			if (command_length == 3)
				is_ascii = args[2];
			else
				is_ascii = " ";
			save_command(img, args[1], is_ascii);
		} else {
			invalid_command_exception();
		}
	}

	free(command);
	free_char_2D_array(args, command_length);
	free_all(img);
	return 0;
}

