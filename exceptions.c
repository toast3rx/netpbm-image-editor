#include <stdio.h>

void no_file_load_exception(void)
{
	printf("No image loaded\n");
}

void invalid_command_exception(void)
{
	printf("Invalid command\n");
}

void invalid_angle_exception(void)
{
	printf("Unsupported rotation angle\n");
}

void memory_allocation_exception(void)
{
	printf("Exception at memory allocation\n");
}

void invalid_coord_exception(void)
{
	printf("Invalid set of coordinates\n");
}

void square_selection_exception(void)
{
	printf("The selection must be square\n");
}

void open_file_exception(char *file_name)
{
	printf("Failed to load %s\n", file_name);
}

void apply_parameter_exception(void)
{
	printf("APPLY parameter invalid\n");
}

void no_file_name_exception(void)
{
	printf("Please enter a file name!\n");
}
