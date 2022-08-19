
#include "include.h"

/*-------------private function declerations-----------*/
bool_err first_pass_file(FILE *f_am);
bool_err first_pass_line(char *line);

bool_err store_symbol(char *symbol_name, command c);
bool_err analyze_command(command c, char *args);
void advance_by_L(char *args);
void advance_by_IC();
bool is_double_register(char *args);

/*---------end of private function declerations-----------*/

bool_err first_pass_main(char *file_name)
{
	/*decleration and initialization*/
	FILE *f_am = NULL;
	bool_err flag = NO_ERR; /*becomes true(ERR) if encounteres an error in pre_process*/

	/*oppening files, creating macro list and initialization of variables*/
	flag |= open_file(&f_am, file_name, AM, READ);
	line_no = 1;

	if (!flag)
		flag = first_pass_file(f_am);/*executed if files opened properly*/

	/*closing files*/
	fclose(f_am);

	return flag;
}

/*--------------------private functions---------------------*/
/*--------------------procedural functions-------------------*/

/*first pass of .am file*/
bool_err first_pass_file(FILE *f_am)
{
	char line[SIZE_LINE + 1];
	bool_err line_flag = NO_ERR;/*on if failed to read line*/
	bool_err flag_pass = NO_ERR;/*on if encountered an error during first_pass*/

	IC = START_ADRESS;/*instruction counter*/
	DC = 0;/*data counter*/

	while (!is_file_empty(f_am))
	{
		line_flag = get_line(line, f_am);
		flag_pass |= line_flag;

		if (!line_flag)
			flag_pass |= first_pass_line(line);

		line_no++;
	}

	advance_by_IC();

	return flag_pass;
}

/*first pass of a single line*/
bool_err first_pass_line(char *line)
{
	bool_err flag = NO_ERR;
	bool is_line_empty;/*true if line is empty*/
	bool is_line_comment;/*true if line is a comment*/

	/*token declerations*/
	char *label_tok = NULL;
	char *args_tok = NULL;
	command command_tok;

	delete_initial_spaces(line);
	delete_final_spaces(line);

	is_line_empty = (*line == '\0');
	is_line_comment = (*line == ';');

	/*checks if line is empty or a comment*/
	if (is_line_empty || is_line_comment)
	{
		line_no++;
		return NO_ERR;
	}

	/*tokenizing line*/
	tokenize_line(line, &label_tok, &command_tok, &args_tok);

	/*ignoring labels in front of extern/entry*/
	if (label_tok && (command_tok == EXTERN_COMMAND || command_tok == ENTRY_COMMAND))
	{
		pwarn(REDUNDANT_LABEL);
		label_tok = NULL;
	}

	/*checks command validity*/
	if (!is_valid_command(command_tok, args_tok))
		return ERR;

	/*analyzing label if there is one*/
	if (label_tok)
		flag |= store_symbol(label_tok, command_tok);

	/*analyze command*/
	flag |= analyze_command(command_tok, args_tok);

	return flag;
}

/*----------------------support functions----------------------*/

/*stores symbol in symbols list*/
bool_err store_symbol(char *symbol_name, command c)
{
	node *n;
	tag t;
	int *data;

	data = malloc(sizeof(int));
	check_alloc(data);

	if (!is_valid_name(symbol_name, symbols))
		return ERR;

	switch (c)
	{
		case MOV: case CMP: case ADD: case SUB: case NOT: case CLR: case LEA: case INC:
		case DEC: case JMP: case BNE: case GET: case PRN: case JSR: case RTS: case HLT:
			t = T_CODE;
			*data = IC;
			break;
		case ARRAY_COMMAND: case STRING_COMMAND: case STRUCT_COMMAND:
			t = T_DATA;
			*data = DC;
			break;
		case EXTERN_COMMAND:
			t = T_EXT;
			data = NULL;
			break;
		default:
			return ERR;
	}

	n = node_create(symbol_name, data, t);
	list_add(symbols, n);
	return NO_ERR;
}

/*analyzing a command line - encoding any .data instructions and advancing IC by L
	upon success returns NO_ERR, else returns ERR*/
bool_err analyze_command(command c, char *args)
{
	delete_initial_spaces(args);
	delete_final_spaces(args);

	switch (c)
	{
		case MOV: case CMP: case ADD: case SUB: case LEA: case NOT: case CLR:  case INC:
		case DEC: case JMP: case PRN: case JSR: case BNE: case GET: case RTS: case HLT:
			advance_by_L(args);
			break;

		case ARRAY_COMMAND:
			return encode_arr(args);

		case STRING_COMMAND:
			return encode_string(args);

		case STRUCT_COMMAND:
			return encode_struct(args);

		case EXTERN_COMMAND:
			return store_symbol(args, EXTERN_COMMAND);

		default:/*does nothing on other cases*/
			break;
	}

	return NO_ERR;
}

/*this function advances IC by L as defined in page 33 in mmn14 instructions*/
void advance_by_L(char *args)
{
	int L;

	IC++;/*advancing for command word*/

	if (!args)/*no args command - no further advancing is needed*/
		;
	else if (is_double_register(args))/*both args are registers - advancing 1 for both registers*/
		IC++;
	else/**/
	{
		L = 1;/*number of words L equals the number of ',' and '.' in args plus 1 for first argument*/
		while (*args)
		{
			if (*args == ',' || *args == '.')
				L++;

			args++;
		}

		IC += L;
	}
}

/*this function advances all data symbols (i.e. excloding code and extern symbols) by IC and returns*/
void advance_by_IC()
{
	node *ptr;
	ptr = symbols->head;

	while (ptr)
	{
		if (ptr->tag == T_DATA)
			*(int *)(ptr->data) += IC;

		ptr = ptr->next;
	}
}

/*true if both args are registers, else false*/
bool is_double_register(char *args)
{
	bool resault;

	/*create a copy of original line*/
	char *args_temp = alloc_str_cpy(args);

	char *arg1 = NULL;/*source*/
	char *arg2 = NULL;/*target*/

	if (args)
	{
		arg1 = strtok(args_temp, ",");
		arg2 = strtok(NULL, "");
	}

	resault = ((get_register(arg1) != NOT_REGISTER) && (get_register(arg2) != NOT_REGISTER));
	free(args_temp);
	return resault;
}