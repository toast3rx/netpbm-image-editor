#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD 12
#define LOAD "LOAD"
#define SELECT "SELECT"
#define SELECT_ALL "SELECT ALL" // minor bug with length of string (MAX_WORD == 10)
#define ROTATE "ROTATE"
#define CROP "CROP"
#define APPLY "APPLY"
#define SAVE "SAVE"
#define EXIT "EXIT"

void read_command(char *command);
void load_file();

/**
 * @brief Used to determine image type
 * 
 */
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
    FILE* file_in;
    image_type type;

    char *file_name;
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
} image_file;

int main(void)
{

    char *command = (char *)malloc((MAX_WORD + 1) * sizeof(char));

    //FILE *in;

    do
    {
        read_command(command);
        if (strcmp(command, LOAD) == 0)
        {
            load_file();
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
    return 0;
}

/**
 * @brief Read a command from stdin
 * 
 * @param command command to be read
 */
void read_command(char *command)
{
    fgets(command, MAX_WORD, stdin);
    command[strlen(command) - 1] = '\0';
}

void load_file()
{
    // Deschidem fisier
    // 
}