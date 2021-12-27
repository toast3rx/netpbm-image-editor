#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD 100
#define MAX_FILENAME 100
#define LOAD "LOAD"
#define SELECT "SELECT"
#define SELECT_ALL "SELECT ALL" // minor bug with length of string (MAX_WORD == 10)
#define ROTATE "ROTATE"
#define CROP "CROP"
#define APPLY "APPLY"
#define SAVE "SAVE"
#define EXIT "EXIT"

typedef enum
{
    PPM,
    PGM
} image_type;

// A pixel
typedef struct
{
    char red, green, blue;
} pixel;

typedef struct
{
    pixel **image;
} ppm_image;

typedef struct
{
    unsigned int value;
} pgm_image;

typedef struct
{
    int rows;
    int cols;
    union
    {
        ppm_image *ppm_zone;
        pgm_image *pgm_zone;
    } selected_zone;
} selection;

typedef struct
{
    FILE *file_in;
    image_type type;

    char magic_number[3];
    int width;
    int height;
    int max_value;
    union
    {
        ppm_image ppm_img;
        pgm_image pgm_img;
    } img;
    selection curr_selection;
} netpbm_image;

/**
 * @brief Split a string in words delimited by space
 * 
 * @param str string to be split
 * @param words_array the array where the words will be stored
 */
void split_string(char *str, char **words_array)
{
    int j = 0;
    int count = 0;
    for (int i = 0; i <= (int)(strlen(str)); i++)
    {
        // if space or NULL found, assign NULL into splitStrings[cnt]
        if (str[i] == ' ' || str[i] == '\0')
        {
            words_array[count][j] = '\0';
            count++; //for next word
            j = 0;   //for next word, init index to 0
        }
        else
        {
            words_array[count][j] = str[i];
            j++;
        }
    }
}

/**
 * @brief Read a command from stdin
 * 
 * @param command command to be read
 * @return arguments of the command
 */
char **read_command(char *command, int *args)
{
    fgets(command, MAX_WORD, stdin);
    command[strlen(command) - 1] = '\0';

    // Count number of words and longest word for memory allocation
    int count_words = 0;
    int max_length = 0;
    int length = 0;

    for (int i = 0; i < (int)strlen(command); i++)
    {
        if (command[i] != ' ')
            length++;
        else
        {
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
    if(!words) {
        printf("Error!\n");
        return NULL;
    }
    for (int i = 0; i < count_words; i++) {
        words[i] = (char *)malloc((max_length + 1) * sizeof(char));
        if(!words[i]) {
            printf("Error\n");
            return NULL;
        }
    }

    split_string(command, words);

    *args = count_words;
    return words;
}

void load_file(char *file_name, netpbm_image *image)
{
    // Deschidem fisier
    // Verific deshidere fisier
    // Citim cifra magica
    // Salvam latimea
    // Salvam lungimea
    // Salvam valoarea maxima pentru pixeli
    // Salvam imaginea ca ppm sau pgm
    // Salvam selectia initial fiind toata imaginea
}

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

void free_2D_array(char **array, int length)
{
    for (int i = 0; i < length; i++)
        free(array[i]);

    free(array);
}

int main(void)
{

    char *command = (char *)malloc((MAX_WORD + 1) * sizeof(char));
    if(!command) {
        printf("Error!\n");
        return -1;
    }
    char **args;
    int command_length = 0;

    do
    {
        if (command_length > 0)
            free_2D_array(args, command_length);
        args = read_command(command, &command_length);
        if (is_load_command(args[0]))
        {
            printf("LOADED\n");
        }
        else if (strcmp(command, SELECT) == 0)
        {
            printf("SELECT!\n");
        }
        else if (strcmp(command, SELECT_ALL) == 0)
        {
            printf("SELECT ALL!\n");
        }
        else if (strcmp(command, ROTATE) == 0)
        {
            printf("ROTATE!\n");
        }
        else if (strcmp(command, CROP) == 0)
        {
            printf("CROP!\n");
        }
        else if (strcmp(command, APPLY) == 0)
        {
            printf("APPLY!\n");
        }
        else if (strcmp(command, SAVE) == 0)
        {
            printf("SAVE!\n");
        }
        else
        {
            printf("Unknow command!\n");
        }

    } while (strcmp(command, EXIT) != 0);

    //fclose(in);
    free(command);
    free_2D_array(args, command_length);
    return 0;
}