#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "utils.h"
#include "constants.h"

void split_string(char *str, char **words_array)
{
	int j = 0;
	int count = 0;
	for (int i = 0; i <= (int)(strlen(str)); i++) {
		if (str[i] == ' ' || str[i] == '\0') {
			words_array[count][j] = '\0';
			count++;
			j = 0;
		} else {
			words_array[count][j] = str[i];
			j++;
		}
	}
}

void check_comments(FILE *in)
{
	char c = getc(in);
	while (c == '#') {
		while (getc(in) != '\n')
			c = getc(in);
	}

	ungetc(c, in);
}

void free_char_2D_array(char **array, int length)
{
	for (int i = 0; i < length; i++)
		free(array[i]);

	free(array);
}

int string_to_int(char *str)
{
	int res = 0;
	int i = 0;
	while (str[i] != '\0') {
		res = res * 10 + (str[i] - '0');
		i++;
	}
	return res;
}

int get_max(int x, int y)
{
	return x > y ? x : y;
}

int get_min(int x, int y)
{
	return x < y ? x : y;
}

void swap(int *x, int *y)
{
	int temp = *x;
	*x = *y;
	*y = temp;
}

void clamp(double *x, int min, int max)
{
	if (*x > max)
		*x = max;
	else if (*x < min)
		*x = min;
}

void move_decimals(double kernel[3][3], int n, int decimal_places)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			kernel[i][j] *= decimal_places;
}
