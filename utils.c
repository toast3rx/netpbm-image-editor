#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "utils.h"

void split_string(char *str, char **words_array)
{
    int j = 0;
    int count = 0;
    for (int i = 0; i <= (int)(strlen(str)); i++) {
        // if space or NULL found, assign NULL into splitStrings[cnt]
        if (str[i] == ' ' || str[i] == '\0') {
            words_array[count][j] = '\0';
            count++; //for next word
            j = 0;   //for next word, init index to 0
        }         else {
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
            ;
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