#include "include.h"

/*printing error massages to stderr*/
/*this file handles with errors*/

/*prints an error massage to stderr and notifies in which line it occured*/
void perr(err e)
{
	char msg[SIZE_MSG];
	switch (e)
	{
		case LINE_LONG:
			strcpy(msg, "line is too long.");
			break;
		case EXTRA_CHAR:
			strcpy(msg, "extra charachters at end of the line");
			break;
		case NAME_LONG:
			strcpy(msg, "macro or lable name is too long");
			break;
		case NAME_TAKEN:
			strcpy(msg, "macro or label name allready defined");
			break;
		case NAME_ILLIGAL_FIRST_CHAR:
			strcpy(msg, "macro or lable name must start with a letter");
			break;
		case NAME_ILLIGAL_CHAR:
			strcpy(msg, "macro or lable name must contain only alpha-numeric charachters");
			break;
		case NAME_RESERVED:
			strcpy(msg, "reserved names cannot be used as macro or lable name");
			break;
		case MACRO_LONG:
			strcpy(msg, "recorded macro is too long");
			break;
		case DOT_ILLIGAL:
			strcpy(msg, "encountered an illigal dot character");
			break;
		case KEYWORD_ILLIGAL:
			strcpy(msg, "command name is illigal");
			break;
		case ARG_NUM_MISMATCH:
			strcpy(msg, "recieved unexpected number of arguments");
			break;
		case DATA_IMAGE_OVERFLOW:
			strcpy(msg, "exceeding capacity of data storage");
			break;
		case NUMBER_ILLIGAL:
			strcpy(msg, "number is illigal");
			break;
		case NUMBER_OUT_OF_BOUND:
			strcpy(msg, "number overflow/underflow");
			break;
		case STRING_ILLIGAL:
			strcpy(msg, "string must be enclosed in double quotes");
			break;
		case STRUCT_ILLIGAL:
			strcpy(msg, "struct must recieve a valid number and a valid string as arguments");
			break;
		case ENTRY_UDEFINED:
			strcpy(msg, "entry symbol name is undefined");
			break;
		case LABEL_UDEFINED:
			strcpy(msg, "label name is undefined");
			break;
		case ADDRESSING_ERROR:
			strcpy(msg, "addressing is illegal");
			break;
	}
	fprintf(stdout, "ERROR in line %d - %s\n", line_no, msg);
}

/*prints a warning massage to stderr and notifies in which line it occured*/
void pwarn(warn w)
{
	char msg[SIZE_MSG];
	switch (w)
	{
		case REDUNDANT_LABEL:
			strcpy(msg, "label at the beginning of extern/entry command is ignored");
			break;

	}
	fprintf(stdout, "WARNING in line %d - %s\n", line_no, msg);
}

/*prints a massage in case of failure in pre-proccess or assembler of a given file*/
void pstatus(stage s, char *file_name)
{
	char msg[SIZE_MSG];
	switch (s)
	{
		case PRE_PROCESS:
			strcpy(msg, "pre-process");
			break;

		case FIRST_PASS:
			strcpy(msg, "first pass");
			break;

		case SECOND_PASS:
			strcpy(msg, "second pass");
			break;
	}
	fprintf(stderr, "FAILURE in %s of file '%s'\n", msg, file_name);
}

/*checking a pointer for null in case of failed memory allocation.*/
/*this function terminates the program and printing an error massage if needed*/
void check_alloc(void *ptr)
{
	if (!ptr)
	{
		fprintf(stderr, "ERROR (line %d) - memory allocation failed.\n Program will be terminated\n", line_no);
		exit(1);
	}
}

/*checking a pointer for null in case of failed file opening.*/
/*this function returns ERR if failed to open a file and printing an error massage*/
bool_err check_fptr(void *ptr)
{
	if (!ptr)
	{
		fprintf(stderr, "ERROR - couldn't open or write into a file\n");
		return ERR;
	}
	return NO_ERR;
}