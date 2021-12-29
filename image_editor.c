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

/**
 * @brief Types of image this application can process
 *
 */
typedef enum
{
    PPM,
    PGM
} image_type;

/**
 * @brief Pixel struct that stores red, green and blue values
 *
 */
typedef struct
{
    unsigned int red, green, blue;
} pixel;

/**
 * @brief PPM image with an array of pixels
 *
 */
typedef struct
{
    pixel** image;
} ppm_image;

/**
 * @brief PHM image with an array of Black/White values
 *
 */
typedef struct
{
    int** values;
} pgm_image;

/**
 * @brief A geomtrical point with x/y coordinates
 *
 */
typedef struct
{
    int x;
    int y;
} point;

/**
 * @brief Select a part of the image given by the coords placed diag opposed
 *
 */
typedef struct
{
    point p1;
    point p2;
} selection;

/**
 * @brief NetPbm image
 *
 */
typedef struct
{
    FILE* file_in;
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
void split_string(char* str, char** words_array)
{
    int j = 0;
    int count = 0;
    for (int i = 0; i <= (int)(strlen(str)); i++) {
        // if space or NULL found, assign NULL into splitStrings[cnt]
        if (str[i] == ' ' || str[i] == '\0') {
            words_array[count][j] = '\0';
            count++; //for next word
            j = 0;   //for next word, init index to 0
        }
        else {
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
char** read_command(char* command, int* args)
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

    char** words = (char**)malloc(count_words * sizeof(char*));
    if (!words) {
        printf("Error!\n");
        return NULL;
    }
    for (int i = 0; i < count_words; i++) {
        words[i] = (char*)malloc((max_length + 1) * sizeof(char));
        if (!words[i]) {
            printf("Error\n");
            return NULL;
        }
    }

    split_string(command, words);

    *args = count_words;
    return words;
}

void check_comments(FILE* in)
{
    char c = getc(in);
    while (c == '#') {
        while (getc(in) != '\n')
            ;
        c = getc(in);
    }

    ungetc(c, in);
}

void free_image(netpbm_image img)
{
    if (img.type == PPM) {
        for (int i = 0; i < img.height; i++)
            free(img.img.ppm_img.image[i]);
        free(img.img.ppm_img.image);
    }
    else if (img.type == PGM) {
        for (int i = 0; i < img.height; i++)
            free(img.img.pgm_img.values[i]);
        free(img.img.pgm_img.values);
    }
    fclose(img.file_in);
}

/**
 * @brief Load an image from stdin
 * 
 * @param file_name File name given by user
 * @param image netpbm struct that stores informations about the file
 */
void load_file(char* file_name, netpbm_image* image)
{   

    if(image->file_in != NULL) {
        free_image(*image);
    }

    image->file_in = fopen(file_name, "rt");
    if (!image->file_in) {
        printf("Cannot open file!\n");
        return;
    }

    check_comments(image->file_in);

    fgets(image->magic_number, 3, image->file_in);
    if (image->magic_number[0] != 'P') {
        printf("Wrong Image format\n");
        return;
    }

    getc(image->file_in); // \n
    check_comments(image->file_in);

    fscanf(image->file_in, "%d", &(image->width));
    fscanf(image->file_in, "%d", &(image->height));

    getc(image->file_in);
    check_comments(image->file_in);

    fscanf(image->file_in, "%d", &(image->max_value));

    if (image->magic_number[1] == '3' || image->magic_number[1] == '6') {
        image->type = PPM;

        image->img.ppm_img.image = (pixel**)malloc(image->height * sizeof(pixel*));
        if (!image->img.ppm_img.image) {
            printf("Error allocation!\n");
            return;
        }
        for (int i = 0; i < image->height; i++) {
            image->img.ppm_img.image[i] = (pixel*)malloc(image->width * sizeof(pixel));
            if (!image->img.ppm_img.image[i]) {
                printf("Error allocation!\n");
                return;
            }
        }
    }
    else if (image->magic_number[1] == '2' || image->magic_number[1] == '5') {
        image->type = PGM;

        image->img.pgm_img.values = (int**)malloc(image->height * sizeof(int*));
        if (!image->img.pgm_img.values) {
            printf("Error allocation!\n");
            return;
        }
        for (int i = 0; i < image->height; i++) {
            image->img.pgm_img.values[i] = (int*)malloc(image->width * sizeof(int));
            if (!image->img.pgm_img.values[i]) {
                printf("Error allocation!\n");
                return;
            }
        }
    }

    getc(image->file_in);
    check_comments(image->file_in);

    // printf("%s\n", image->magic_number);
    // printf("%d\n", image->width);
    // printf("%d\n", image->height);
    // printf("%d\n", image->max_value);
    if (image->type == PPM) {
        if (image->magic_number[1] == '3') {
            for (int i = 0; i < image->height; i++) {
                for (int j = 0; j < image->width; j++) {
                    fscanf(image->file_in, "%d", &(image->img.ppm_img.image[i][j].red));
                    fscanf(image->file_in, "%d", &(image->img.ppm_img.image[i][j].green));
                    fscanf(image->file_in, "%d", &(image->img.ppm_img.image[i][j].blue));
                }
            }
        }
        else {
            for (int i = 0; i < image->height; i++)
                for (int j = 0; j < image->height; j++) {
                    fread(&(image->img.ppm_img.image[i][j].red), sizeof(int), 1, image->file_in);
                    fread(&(image->img.ppm_img.image[i][j].green), sizeof(int), 1, image->file_in);
                    fread(&(image->img.ppm_img.image[i][j].blue), sizeof(int), 1, image->file_in);
                }
        }
    }
    else if (image->type == PGM) {
        if (image->magic_number[1] == '2')
            for (int i = 0; i < image->height; i++)
                for (int j = 0; j < image->height; j++) {
                    fscanf(image->file_in, "%d", &(image->img.pgm_img.values[i][j]));
                }
        else
            for (int i = 0; i < image->height; i++)
                for (int j = 0; j < image->height; j++) {
                    fread(&(image->img.pgm_img.values[i][j]), sizeof(int), 1, image->file_in);
                }
    }

    // Save current selection as entire image
    image->curr_selection.p1.x = 0;
    image->curr_selection.p1.y = 0;
    image->curr_selection.p2.x = image->width - 1;
    image->curr_selection.p2.y = image->height - 1;
}

/**
 * @brief
 * Check if inserted command is the LOAD command
 * @param command input from stdin
 * @return 1 for true, 0 for false
 */
int is_load_command(char* command)
{
    if (strcmp(command, LOAD) == 0)
        return 1;
    else
        return 0;
}

/**
 * @brief Dealocate memory a 2D array of char
 * 
 * @param array Array to be deallocated
 * @param length number of rows
 */
void free_char_2D_array(char** array, int length)
{
    for (int i = 0; i < length; i++)
        free(array[i]);

    free(array);
}



int main(void)
{

    char* command = (char*)malloc((MAX_WORD + 1) * sizeof(char));
    if (!command) {
        printf("Error!\n");
        return -1;
    }
    char** args;
    int command_length = 0;

    netpbm_image img;
    img.file_in = NULL;

    while (1) {
        if (command_length > 0)
            free_char_2D_array(args, command_length);
        args = read_command(command, &command_length);
        if (strcmp(command, EXIT) == 0)
            break;
        else if (is_load_command(args[0])) {
            load_file(args[1], &img);
        }
        else if (strcmp(command, SELECT) == 0) {
            printf("SELECT!\n");
        }
        else if (strcmp(command, SELECT_ALL) == 0) {
            printf("SELECT ALL!\n");
        }
        else if (strcmp(command, ROTATE) == 0) {
            printf("ROTATE!\n");
        }
        else if (strcmp(command, CROP) == 0) {
            printf("CROP!\n");
        }
        else if (strcmp(command, APPLY) == 0) {
            printf("APPLY!\n");
        }
        else if (strcmp(command, SAVE) == 0) {
            printf("SAVE!\n");
        }
        else {
            printf("Unknow command!\n");
        }
    }

    free(command);
    free_char_2D_array(args, command_length);
    
    free_image(img);
    return 0;
}

