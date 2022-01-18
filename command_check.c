#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "structs.h"
#include "utils.h"
#include "exceptions.h"

int is_load_command(char *command)
{
	if (strcmp(command, LOAD) == 0)
		return 1;
	else
		return 0;
}

int is_select_command(char *command)
{
	if (strcmp(command, SELECT) == 0)
		return 1;
	else
		return 0;
}

int is_select_all_command(char *command, char *arg)
{
	if (is_select_command(command)) {
		if (arg[3] == ' ')
			arg[3] = '\0';
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

	invalid_angle_exception();
	return 0;
}

int is_exit_command(char *command, netpbm_format img)
{
	if (strcmp(command, EXIT) == 0) {
		if (!img.file_in)
			no_file_load_exception();
		return 1;
	}

	return 0;
}

int is_crop_command(char *command)
{
	if (strcmp(command, CROP) == 0)
		return 1;

	return 0;
}

int is_apply_filter_command(char *command)
{
	if (strcmp(command, APPLY) == 0)
		return 1;

	return 0;
}

int is_save_command(char *command)
{
	if (strcmp(command, SAVE) == 0)
		return 1;

	return 0;
}
