
#include "include.h"

#define SIZE_EXTENSION 5/*maximum size of file extension (inncluding '.' and '\0' chaars)*/

int line_no;/*global variable that tracks the current line in a files*/
int IC;/*instructions counter*/
int DC;/*data counter*/
list *symbols;/*symbol list for first/second pass*/
int data_image[SIZE_DATA_IMAGE];/*list of data encoded in binary*/
int num_ocupide_data_cells;/*holds the number of occupied data cells in data_image*/

/*------------private function declerations-----------*/
void notify_start(char *file_name);
void notify_end(char *file_name, int flag);
bool is_argc_empty(int argc);
bool_err execute_stage(char *file_name, stage s, bool_err flag);
/*-------end of private function declerations-----------*/

int main(int argc, char *argv[])
{
	int i;

	if (is_argc_empty(argc))
		exit(0);
	
	for (i = 1; i < argc; i++)
	{
		bool_err flag = NO_ERR;
		char *file_name = malloc((strlen(argv[i]) + SIZE_EXTENSION) * sizeof(char));
		check_alloc(file_name);
		symbols = list_create();
		num_ocupide_data_cells = 0;

		strcpy(file_name, argv[i]);
		notify_start(file_name);
		flag = execute_stage(file_name, PRE_PROCESS, flag);
		flag = execute_stage(file_name, FIRST_PASS, flag);
		flag = execute_stage(file_name, SECOND_PASS, flag);
		
		notify_end(file_name, flag);
		free(file_name);
		list_delete(symbols);
	}

	return 0;
}

void notify_start(char *file_name)
{
	printf("Starting assembler for file %s\n", file_name);
}
void notify_end(char *file_name, int flag)
{
	if (flag)
		printf("assembler for file %s failed\n", file_name);
	else
		printf("assembler for file %s successful\n", file_name);
	printf("------------------------------------------\n");
}

/*this function performs the algorithm of the given stage and passes a list pointer to store data between stages (if needed)
	it returns NO_ERR upon complition or ERR in case of faliure and prints a corresponding notification*/
bool_err execute_stage(char *file_name, stage s, bool_err flag_main)
{
	bool_err flag_stage = NO_ERR;/*holds ERR(1) if encounters error, else holds NO_ERR(0)*/
	bool_err (*stage_algorithm)(char *) = NULL;/*hold a pointer to the stage algorithm function*/

	if (flag_main)
		return ERR;

	switch (s)
	{
		case PRE_PROCESS:
			stage_algorithm = &pre_process_main;
			break;
		case FIRST_PASS:
			stage_algorithm = &first_pass_main;
			break;
		case SECOND_PASS:
			stage_algorithm = &second_pass_main;
			break;
	}

	flag_stage = stage_algorithm(file_name);/*activating the preprocess algorithm*/

	if (flag_stage)
	{
		pstatus(s, file_name);
		return ERR;
	}

	return NO_ERR;
}

bool is_argc_empty(int argc)
{
	if (argc == 1)/*no command line arguments*/
	{
		fprintf(stderr, "No files pased. Please pass file names as command line arguments!\n");
		fprintf(stderr, "Terminating program\n");
		return TRUE;
	}

	return FALSE;
}