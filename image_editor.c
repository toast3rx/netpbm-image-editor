#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_WORD 100
#define MAX_FILENAME 100
#define LOAD "LOAD"
#define SELECT "SELECT"
#define SELECT_ALL "ALL" // minor bug with length of string (MAX_WORD == 10)
#define ROTATE "ROTATE"
#define CROP "CROP"
#define APPLY "APPLY"
#define SAVE "SAVE"
#define EXIT "EXIT"

#define EDGE_FILTER "EDGE"
#define SHARPEN_FILTER "SHARPEN"
#define BLUR_FILTER "BLUR"
#define GAUSSIAN_BLUR_FILTER "GAUSSIAN"

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
    pixel** pixels;
} image;

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
    image picture;
    selection curr_selection;
} netpbm_format;

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

void free_image(image img, int height)
{
    for (int i = 0; i < height; i++)
        free(img.pixels[i]);
    free(img.pixels);
}

void free_all(netpbm_format img)
{
    if (img.file_in) {
        free_image(img.picture, img.height);
        fclose(img.file_in);
    }
}

/**
 * @brief Load an image from stdin
 *
 * @param file_name File name given by user
 * @param image netpbm struct that stores informations about the file
 */
void load_file(char* file_name, netpbm_format* netpbm_img)
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
    }
    else if (netpbm_img->magic_number[1] == '2' || netpbm_img->magic_number[1] == '5') {
        netpbm_img->type = PGM;
    }

    // Alloc
    netpbm_img->picture.pixels = (pixel**)malloc(netpbm_img->height * sizeof(pixel*));
    if (!netpbm_img->picture.pixels) {
        printf("Error allocation!\n");
        return;
    }
    for (int i = 0; i < netpbm_img->height; i++) {
        netpbm_img->picture.pixels[i] = (pixel*)malloc(netpbm_img->width * sizeof(pixel));
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
        }
        else {
            for (int i = 0; i < netpbm_img->height; i++)
                for (int j = 0; j < netpbm_img->width; j++) {
                    fread(&(netpbm_img->picture.pixels[i][j].red), sizeof(int), 1, netpbm_img->file_in);
                    fread(&(netpbm_img->picture.pixels[i][j].green), sizeof(int), 1, netpbm_img->file_in);
                    fread(&(netpbm_img->picture.pixels[i][j].blue), sizeof(int), 1, netpbm_img->file_in);
                }
        }
    }
    else if (netpbm_img->type == PGM) {
        if (netpbm_img->magic_number[1] == '2')
            for (int i = 0; i < netpbm_img->height; i++)
                for (int j = 0; j < netpbm_img->width; j++) {
                    int value;
                    fscanf(netpbm_img->file_in, "%d", &value);
                    netpbm_img->picture.pixels[i][j].red = value;
                    netpbm_img->picture.pixels[i][j].green = value;
                    netpbm_img->picture.pixels[i][j].blue = value;
                }
        else
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

/**
 * @brief Check for select command
 *
 * @param command given by the user
 * @return int true or false
 */
int is_select_command(char* command)
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
int is_select_all_command(char* command, char* arg)
{
    if (is_select_command(command)) {
        if (strcmp(arg, SELECT_ALL) == 0)
            return 1;
    }

    return 0;
}

/**
 * @brief Convert a string number to an int
 *
 * @param str number
 * @return int number converted
 */
int string_to_int(char* str)
{
    int res = 0;
    int i = 0;
    while (str[i] != '\0') {
        res = res * 10 + (str[i] - '0');
        i++;
    }
    return res;
}

/**
 * @brief Restrict current selection to the given index
 *
 * @param args array with command arguments
 * @param image netpbm image
 */
void select_command(char** args, netpbm_format* image)
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

/**
 * @brief Select all image as current selection
 *
 * @param img netpbm image
 */
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

int get_max(int x, int y)
{
    return x > y ? x : y;
}

int get_min(int x, int y)
{
    return x < y ? x : y;
}


/**
 * Transpose matrix in place
 * @param mat - 2D array to be tranposed
 * @param size - number for rows/columns
 */
void transpose_square_matrix(image img, int start_row, int end_row, int end_col)
{
    for (int i = start_row; i < end_row; ++i) {
        for (int j = i; j < end_col; ++j) {
            int red_tmp = img.pixels[i][j].red;
            int green_tmp = img.pixels[i][j].green;
            int blue_tmp = img.pixels[i][j].blue;

            img.pixels[i][j].red = img.pixels[j][i].red;
            img.pixels[i][j].green = img.pixels[j][i].green;
            img.pixels[i][j].blue = img.pixels[j][i].blue;

            img.pixels[j][i].red = red_tmp;
            img.pixels[j][i].green = green_tmp;
            img.pixels[j][i].blue = blue_tmp;
        }
    }
}

void swap(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void transpose_image(image* img, int* height, int* width)
{

    if (*height != *width) {

        int max = get_max(*height, *width);
        //img.pixels = (pixel **)realloc(img.pixels, max * sizeof(pixel *));
        if (*width > *height) {

            (*img).pixels = (pixel**)realloc((*img).pixels, *width * sizeof(pixel*));
            for (int i = *height; i < *width; i++) {
                (*img).pixels[i] = (pixel*)calloc(max, sizeof(pixel));
            }

            transpose_square_matrix((*img), 0, max, max);

            for (int i = 0; i < max; i++)
                (*img).pixels[i] = realloc((*img).pixels[i], *height * sizeof(pixel));
        }
        else {
            for (int i = 0; i < *height; i++)
                (*img).pixels[i] = realloc((*img).pixels[i], *height * sizeof(pixel));
            transpose_square_matrix((*img), 0, max, max);

            for (int i = *width; i < *height; i++)
                free((*img).pixels[i]);

            (*img).pixels = realloc((*img).pixels, *width * sizeof(pixel));
        }

        swap(height, width);
        printf("W%d H%d\n", *width, *height);
    }
    else
        transpose_square_matrix((*img), 0, *height, *width);
}

void reverse_rows(image img, int start_row, int end_row, int start_col, int end_col)
{
    for (int i = start_row; i < end_row; i++) {
        int low = start_col;
        int high = end_col - 1;
        while (low < high) {
            int red_temp = img.pixels[i][low].red;
            int green_temp = img.pixels[i][low].green;
            int blue_temp = img.pixels[i][low].blue;

            img.pixels[i][low].red = img.pixels[i][high].red;
            img.pixels[i][low].green = img.pixels[i][high].green;
            img.pixels[i][low].blue = img.pixels[i][high].blue;

            img.pixels[i][high].red = red_temp;
            img.pixels[i][high].green = green_temp;
            img.pixels[i][high].blue = blue_temp;

            low++;
            high--;
        }
    }
}
void reverse_cols(image img, int start_row, int end_row, int start_col, int end_col)
{
    for (int i = start_col; i < end_col; i++) {
        int low = start_row;
        int high = end_row - 1;
        while (low < high) {
            int red_temp = img.pixels[low][i].red;
            int green_temp = img.pixels[low][i].green;
            int blue_temp = img.pixels[low][i].blue;

            img.pixels[low][i].red = img.pixels[high][i].red;
            img.pixels[low][i].green = img.pixels[high][i].green;
            img.pixels[low][i].blue = img.pixels[high][i].blue;

            img.pixels[high][i].red = red_temp;
            img.pixels[high][i].green = green_temp;
            img.pixels[high][i].blue = blue_temp;

            low++;
            high--;
        }
    }
}
void rotate_image_clockwise(netpbm_format* image, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        //transpose
        transpose_image((&(*image).picture), &(*image).height, &(*image).width);
        (*image).curr_selection.p2.x = (*image).width;
        (*image).curr_selection.p2.y = (*image).height;
        // reverse
        reverse_rows((*image).picture, 0, (*image).height, 0, (*image).width);
    }
}

void rotate_image_counter_clockwise(netpbm_format* image, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        //transpose
        transpose_image((&(*image).picture), &(*image).height, &(*image).width);
        (*image).curr_selection.p2.x = (*image).width;
        (*image).curr_selection.p2.y = (*image).height;
        // reverse
        reverse_cols(image->picture, 0, image->height, 0, image->width);
    }
}
void rotate_selection_clockwise(netpbm_format* image, int start_row, int start_col, int end_row, int end_col, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        transpose_square_matrix(image->picture, start_row, end_row, end_col);
        reverse_rows(image->picture, start_row, end_row, start_col, end_col);
    }
}

void rotate_selection_counter_clockwise(netpbm_format* image, int start_row, int start_col, int end_row, int end_col, int angle)
{
    for (int k = 0; k < (angle / 90); k++) {
        transpose_square_matrix(image->picture, start_row, end_row, end_col);
        reverse_cols(image->picture, start_row, end_row, start_col, end_col);
    }
}

void rotate_command(netpbm_format* image, char* angle)
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
    }
    else if (end_row - start_row == end_col - start_col) {
        // rotate selection
        if (sign == '+') {
            rotate_selection_clockwise(image, start_row, start_col, end_row, end_col, angle_converted);
        }
        else if (sign == '-') {
            rotate_selection_counter_clockwise(image, start_row, start_col, end_row, end_col, angle_converted);
        }
    }
    else {
        printf("The selection must be square\n");
        return;
    }

    printf("Rotated %c%d\n", sign, angle_converted);

}

int is_rotate_command(char** args, int length)
{
    if (strcmp(args[0], ROTATE) != 0)
        return 0;
    if (length == 2 && args[1] != NULL)
        return 1;
    else {
        printf("Unsupported rotation angle!\n");
        return 0;
    }
}

void crop_command(netpbm_format* img)
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

    selected_img.pixels = (pixel**)malloc(selection_height * sizeof(pixel*));
    if (!selected_img.pixels) {
        printf("Error allocation!\n");
        return;
    }
    for (int i = 0; i < selection_height; i++) {
        selected_img.pixels[i] = (pixel*)calloc(selection_width, sizeof(pixel));
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

void print_image(netpbm_format img)
{
    printf("%s\n%d %d\n%d\n", img.magic_number, img.width, img.height, img.max_value);
    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {
            printf("%d %d %d       ", img.picture.pixels[i][j].red, img.picture.pixels[i][j].green, img.picture.pixels[i][j].blue);
        }
        printf("\n");
    }
}

void clamp(double* x, int min, int max)
{
    if (*x > max)
        *x = max;
    else if (*x < min)
        *x = min;
}

void apply_filter(netpbm_format* img, double kernel[3][3], double fraction)
{
    int delimiter = 3;

    int start_row = get_min(img->curr_selection.p1.y, img->curr_selection.p2.y);
    int end_row = get_max(img->curr_selection.p1.y, img->curr_selection.p2.y);
    int start_col = get_min(img->curr_selection.p1.x, img->curr_selection.p2.x);
    int end_col = get_max(img->curr_selection.p1.x, img->curr_selection.p2.x);
    // copy image
    image temp;
    temp.pixels = (pixel**)malloc(img->height * sizeof(pixel *));
    for(int i = 0; i < img->height; i++) 
        temp.pixels[i] = (pixel *)malloc(img->width * sizeof(pixel));

    for(int i = 0; i < img->height; i++)
        for(int j = 0; j < img->width; j++) {
            temp.pixels[i][j].red = img->picture.pixels[i][j].red;
            temp.pixels[i][j].green = img->picture.pixels[i][j].green;
            temp.pixels[i][j].blue = img->picture.pixels[i][j].blue;
        }
    
    for (int i = start_row; i < end_row; i++) {
        if (i == 0 || i == img->height - 1)
            continue;
        for (int j = start_col; j < end_col; j++) {
            if (j == 0 || j == img->width - 1)
                continue;

            double new_red = 0;
            double new_green = 0;
            double new_blue = 0;

            for (int kernel_row = 0; kernel_row < delimiter; kernel_row++)
                for (int kernel_col = 0; kernel_col < delimiter; kernel_col++) {
                    new_red += (kernel[kernel_row][kernel_col] * temp.pixels[i - 1 + kernel_row][j - 1 + kernel_col].red) / fraction;
                    new_green += (kernel[kernel_row][kernel_col] * temp.pixels[i - 1 + kernel_row][j - 1 + kernel_col].green) / fraction;
                    new_blue += (kernel[kernel_row][kernel_col] * temp.pixels[i - 1 + kernel_row][j - 1 + kernel_col].blue) / fraction;
                }
            // new_red /= fraction;
            // new_blue /= fraction;
            // new_red /= fraction;

            clamp(&new_red, 0, 255);
            clamp(&new_blue, 0, 255);
            clamp(&new_green, 0, 255);
            img->picture.pixels[i][j].red = ceil(new_red);
            img->picture.pixels[i][j].green = ceil(new_green);
            img->picture.pixels[i][j].blue = ceil(new_blue);
        }
    }

    free_image(temp, img->height);

}

void apply_command(netpbm_format* img, char* filter)
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
    }
    else if (strcmp(filter, SHARPEN_FILTER) == 0) {
        double sharpen_kernel[3][3] = {
            {0.0, -1.0, 0.0},
            {-1.0, 5.0, -1.0},
            {0.0, -1.0, 0.0}
        };
        apply_filter(img, sharpen_kernel, 1);
    }
    else if (strcmp(filter, BLUR_FILTER) == 0) {
        double blur_kernel[3][3] = {
            {1.0, 1.0, 1.0},
            {1.0, 1.0, 1.0},
            {1.0, 1.0, 1.0}
        };
        apply_filter(img, blur_kernel, 9.0);
    }
    else if (strcmp(filter, GAUSSIAN_BLUR_FILTER) == 0) {
        double gaussain_blur_kernel[3][3] = {
            {0.0625, 0.125, 0.0625},
            {0.125, 0.25 , 0.125},
            {0.0625, 0.125, 0.0625}
        };
        apply_filter(img, gaussain_blur_kernel, 1);
    }
    else {
        printf("APPLY paramter invalid\n");
        return;
    }

    printf("APPLY %s done\n", filter);
}

void save_command(netpbm_format img, char* file_name, char *is_ascii)
{   
    ///MAGIC NUMBER CHANGE
    int ascii = 0;
    if(strcmp(is_ascii, "ascii") == 0) 
        ascii = 1;

    FILE* fin;
    fin = fopen(file_name, "wt");
    if (!fin) {
        printf("Error opening file\n");
        return;
    }

    char ppm_no[2] = {'6', '3'};
    char pgm_no[2] = {'5', '2'};
    char magic_number;
    if(img.type == PPM)
        magic_number = ppm_no[ascii];
    else if(img.type == PGM)
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
    }
    else {
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
                }
                else
                    fwrite(&(img.picture.pixels[i][j].red), sizeof(char), 1, fin);
            }
            //fwrite("\n", sizeof(char), 1, fin);
        }
    }

    fclose(fin);
    printf("Saved %s\n", file_name);
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

    netpbm_format img;
    img.file_in = NULL;

    while (1) {
        if (command_length > 0)
            free_char_2D_array(args, command_length);
        args = read_command(command, &command_length);
        if (strcmp(command, EXIT) == 0)
            break;
        else if (is_load_command(args[0])) {
            if (args[1] != NULL)
                load_file(args[1], &img);
            else
                printf("Please enter a file name!\n");
        }
        else if (is_select_command(args[0])) {
            if (command_length == 2 && is_select_all_command(args[0], args[1]))
                printf("SELECT ALL!\n");
            else
                if (command_length != 5)
                    printf("Invalid number of coordinates!\n");
                else
                    select_command(args, &img);
        }
        else if (is_rotate_command(args, command_length)) {
            rotate_command(&img, args[1]);
        }
        else if (strcmp(command, CROP) == 0) {
            crop_command(&img);
        }
        else if (strcmp(args[0], APPLY) == 0) {
            apply_command(&img, args[1]);
        }
        else if (strcmp(args[0], SAVE) == 0) {
            char *is_ascii;
            if(command_length == 3)
                is_ascii = args[2];
            else
                is_ascii = " ";
            save_command(img, args[1], is_ascii);
        }
        else if (strcmp(command, "PRINT") == 0) {
            print_image(img);
        }
        else {
            printf("Unknow command!\n");
        }
    }

    free(command);
    free_char_2D_array(args, command_length);

    free_all(img);
    return 0;
}

