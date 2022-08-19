
#ifndef INCLUDE
#define INCLUDE

#pragma warning(disable : 4996)

/*------------typedefs used through the entire project-----------------*/

typedef enum bool{
	FALSE = 0,
	TRUE = 1
} bool;
typedef enum bool_err{
	NO_ERR = 0,
	ERR = 1
} bool_err;

/*----------------------------------includes------------------------------*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "list.h"
#include "err.h"
#include "util.h"
#include "data_image.h"
#include "syntax_analysis.h"

#define SIZE_LINE 81 /*max line size including \n*/
#define SIZE_MACRO (SIZE_LINE * 30)/*max size of macro data*/
#define SIZE_DATA_IMAGE 100/*max size of words that can be stored in data_image*/
#define START_ADRESS 100/*first adress available for writing*/
#define NUM_DIGITS 3/*number of digits required to represent 10 bits in base 32 plus 1 for '\0' char*/

/*min/max number of imaginary computer (10-bit signed integer)*/
#define MAX_NUM 511
#define MIN_NUM -512

/*-------------------------------------external variables--------------------------*/
extern int line_no;/*global variable that tracks the current line in a file*/
extern int IC;/*instructions counter*/
extern int DC;/*data counter*/
extern list *symbols;/*symbol list for first/second pass*/
extern int data_image[SIZE_DATA_IMAGE];/*list of data encoded in binary*/
extern int num_ocupide_data_cells;/*holds the number of occupied data cells in data_image*/

/*----------------------declerations of stage algorithem functions--------------------*/
bool_err pre_process_main(char *file_name);
bool_err first_pass_main(char *file_name);
bool_err second_pass_main(char *file_name);

#endif /*!INCLUDE*/