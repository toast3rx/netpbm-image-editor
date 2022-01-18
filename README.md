##### #©Copyright Latea Mihai Alexandru, 311CA 2022
# NETPBM image editor for PPM and PGM files
<div style = "float:right"> 
PPM image before and after edge filter
<img title = "jake" src = "image_example/jake_ba.jpeg" style = "display:block">
</div>

* This application works with ppm and pgm files from the netpbm format.
* It can load files, from user input, crop, rotate the image and apply\
different filters for a custom selection.
* The user can choose to save the new image as *binary* or *acsii*.
* The image is stored in a structure that divides the file in multiple\
data: magic number, width, height, current selection, values.
* The values are stored in a array of structures named `pixel`\
that contains 3 integer values named `red, green, blue`.
* If the image is in gray format, all three values will have the same number.

## Implementation:
1. There are several commands that are allowed in the editor:
    * LOAD command
	* SELECT command
	* SELECT ALL command
	* ROTATE command
	* CROP command
	* APPLY filter command
	* SAVE command
1. LOAD:
    * This command is applied when user inserts from keyboard\
	command `LOAD *file_name*`;
	* The image is loaded in the application, saving all useful information \
	like its magic number, width and height, pixel values.
	* When pixels value are read, in the application, values are multiplied \
  		by a `PRECISION` constant. To reduce arithmetic errors from \
		filters kernels, we move desired number of decimal places \
		in the left of the comma to work with `int` data types.
2. SELECT:
    * Is called when user inserts the format `SELECT *x1 y1 x2 y2`\
       where x and y are coordinates for 2 points that determines\
	   the rectangle that will be selected from the image.
4. SELECT ALL:
    * This command derives from the previous one but the format is\
		`SELECT ALL` ;
    * It selects all image.
5. ROTATE:
    * Is called when user inserts the format `ROTATE *angle*`
    * It rotates the image clockwise/counter-clockwise\
       (angle can be smaller than 0) by rotating it multiple times\
	   with 90 degrees;
	* For rotating by 90 degrees, the selection is first transposed and then\
		elements from every *row* are reversed.
	* For rotating by -90 degrees, the selection is first transposed and then\
		elements from every *column* are reversed;
    * Accepted angles are **±90/±180/±270/±360**.
6. CROP:
    * Is called when user inserts the format `CROP`
    * It reduces the image to current selection.
7. APPLY:
	 * Is called when user inserts the format `APPLY *filter_name*`
	 * Accepted filters right now are: **Edge, Sharpen, Blur, Gaussian Blur**
	 * Every value from the kernel is multiplied by a constant `DECIMAL_PLACES`\
	 * to work only with `int` data types as mentioned at LOAD command.
8. SAVE:
	 * Is called when user inserts format `Save "file_name" <ascii>"`;
	 * The user can choose to save the image in ascii, default is binary;
	 * Before writing values in file, they are divided by `PRECISION`\
	 	 and rounded to get closest integer after filters formulas.
	
     

