
#define SIZE_MSG 100 /*max size of error massage*/

/*defines all syntatic errors that can occures in .as and .am files*/
typedef enum err {
	LINE_LONG, /*line is too long*/
	EXTRA_CHAR, /*there are extra charachters at the end of the line*/

	NAME_LONG, /*given macro/lable name is too long*/
	NAME_TAKEN, /*macro/lable name is taken*/
	NAME_ILLIGAL_FIRST_CHAR, /*macro/lable name must start with a letter*/
	NAME_ILLIGAL_CHAR, /*macro/lable name must contain only alpha-numeric charachters*/
	NAME_RESERVED, /*reserved names cannot be used as macro/lable name*/

	MACRO_LONG, /*recorded macro is too long*/

	KEYWORD_ILLIGAL, /*illigal keyword command*/
	ARG_NUM_MISMATCH, /*number of arguments not matching expected number*/
	DATA_IMAGE_OVERFLOW,/*trying to store more data when data_image is full*/
	NUMBER_ILLIGAL,/*recieved illigal number format*/
	NUMBER_OUT_OF_BOUND,/*number overflow/underflow*/
	STRING_ILLIGAL,/*illigal string format*/
	STRUCT_ILLIGAL,/*illigal struct format*/
	ENTRY_UDEFINED,/*entry name is undefined*/
	LABEL_UDEFINED,/*label name is undefined*/

	DOT_ILLIGAL, /*illigal '.' char in argument*/
	ADDRESSING_ERROR
}err;

typedef enum warn {
	REDUNDANT_LABEL /*lable in beginning of extern/entry command*/
}warn;

typedef enum stage {
	PRE_PROCESS,
	FIRST_PASS,
	SECOND_PASS
}stage;

void perr(err e);
void pwarn(warn w);
void pstatus(stage s, char *file_name);
void check_alloc(void *ptr);
bool_err check_fptr(void *ptr);
