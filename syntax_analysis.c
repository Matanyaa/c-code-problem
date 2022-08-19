#include "include.h"
/*---------------this file contains common assembly syntax tools----------*/
/*------------------------------------------------------------------------*/
bool is_label(char *line);
int count_args(char *args);
int get_arg_num_expected(command k);

bool is_immidiat(char *arg);
bool is_direct(char *arg);
bool is_struct(char *arg);
bool is_register(char *arg);
bool is_keyword(char *name);
/*---------------------------syntax analysis functions---------------------*/
/*tokenizing a line*/
void tokenize_line(char *line, char **label_tok, command *command_tok, char **args_tok)
{
	char *command_str = NULL;

	if (is_label(line))
	{
		*label_tok = strtok(line, ":");
		command_str = strtok(NULL, " \t\r\n");
		*args_tok = strtok(NULL, "\r\n");
	}
	else
	{
		*label_tok = NULL;
		command_str = strtok(line, " \t\r\n");
		*args_tok = strtok(NULL, "\r\n");
	}

	*command_tok = get_command(command_str);

}

/*returns keyword enum if word is a keyword (excluding registers), else returns NO_KEYWORD enum*/
command get_command(char *word)
{
	/*commands*/
	if (!strcmp(word, "mov"))
		return MOV;
	if (!strcmp(word, "cmp"))
		return CMP;
	if (!strcmp(word, "add"))
		return ADD;
	if (!strcmp(word, "sub"))
		return SUB;
	if (!strcmp(word, "not"))
		return NOT;
	if (!strcmp(word, "clr"))
		return CLR;
	if (!strcmp(word, "lea"))
		return LEA;
	if (!strcmp(word, "inc"))
		return INC;
	if (!strcmp(word, "dec"))
		return DEC;
	if (!strcmp(word, "jmp"))
		return JMP;
	if (!strcmp(word, "bne"))
		return BNE;
	if (!strcmp(word, "get"))
		return GET;
	if (!strcmp(word, "prn"))
		return PRN;
	if (!strcmp(word, "jsr"))
		return JSR;
	if (!strcmp(word, "rts"))
		return RTS;
	if (!strcmp(word, "hlt"))
		return HLT;

	/*instructions*/
	if (!strcmp(word, ".data"))
		return ARRAY_COMMAND;
	if (!strcmp(word, ".string"))
		return STRING_COMMAND;
	if (!strcmp(word, ".struct"))
		return STRUCT_COMMAND;

	/*extern/entry*/
	if (!strcmp(word, ".extern"))
		return EXTERN_COMMAND;
	if (!strcmp(word, ".entry"))
		return ENTRY_COMMAND;

	return NO_COMMAND;
}

/*returns the register number represented by word, if no register is reprisented returns NOT_REGISTER*/
unsigned int get_register(char *arg)
{
	int register_num;

	delete_initial_spaces(arg);
	delete_final_spaces(arg);

	if (!arg || (strlen(arg) != REGISTER_NAME_STR_LENGTH) || (*arg != 'r'))
		return NOT_REGISTER;

	register_num = atoi(arg + 1);

	if (register_num >= NUM_REGISTERS || register_num < 0)
		return NOT_REGISTER;

	return (unsigned int)register_num;
}


/*-----------------------syntax validation--------------------------*/
/*checks if the given name is valid for lable/macro name*/
bool is_valid_name(char *name, list *l)
{
	int i;
	/*checks if name has white spaces*/
	if (strchr(name, ' ') != NULL || strchr(name, '\t') != NULL)
	{
		perr(EXTRA_CHAR);
		return FALSE;
	}

	/*checks that name is not taken*/
	if (list_find(l, name))
	{
		perr(NAME_TAKEN);
		return FALSE;
	}

	/*checks length of name*/
	if (strlen(name) > SIZE_NAME)
	{
		perr(NAME_LONG);
		return FALSE;
	}

	/*checks first charachter*/
	if (!isalpha(*name))
	{
		perr(NAME_ILLIGAL_FIRST_CHAR);
		return FALSE;
	}

	/*checks all chars are alnums*/
	for (i = 0; i < strlen(name); i++)
		if (!isalnum(*(name + i)))
		{
			perr(NAME_ILLIGAL_CHAR);
			return FALSE;
		}

	/*checks against keywords and registers*/
	if (is_keyword(name))
	{
		perr(NAME_RESERVED);
		return FALSE;
	}

	return TRUE;
}

/*checks if str is a valid number*/
bool is_valid_num(char *str)
{
	int num;

	if (*str == '-' || *str == '+')
		str++;

	while (!*str)
	{
		if (!isdigit(*str))
		{
			perr(NUMBER_ILLIGAL);
			return FALSE;
		}
		str++;
	}

	num = atoi(str);

	if (num < MIN_NUM || num > MAX_NUM)
	{
		perr(NUMBER_OUT_OF_BOUND);
		return FALSE;
	}

	return TRUE;
}

/*checks if k is a valid command and has enough arguments*/
bool is_valid_command(command c, char *args)
{
	int arg_num = count_args(args);
	int arg_num_expected = get_arg_num_expected(c);

	if (c == NO_COMMAND)
	{
		perr(KEYWORD_ILLIGAL);
		return FALSE;
	}
	else if (c == ARRAY_COMMAND)
	{
		return arg_num > 0;
	}
	else if (arg_num_expected != arg_num)
	{
		perr(ARG_NUM_MISMATCH);
		return FALSE;
	}

	return TRUE;
}

/*checks if str is a valid string (i.e. encased in double quotes)*/
bool is_valid_string(char *str)
{
	return (*str == '\"' && *(str + strlen(str) - 1) == '\"');
}

bool is_valid_struct(char *str)
{
	char *name;
	char *field;
	char *str_temp = alloc_str_cpy(str);

	field = strchr(str_temp, '.') + 1;
	name = strtok(str_temp, ".");

	if (*field != '1' && *field != '2')
	{
		free(str_temp);
		perr(STRUCT_ILLIGAL);
		return FALSE;
	}
	if (!list_find(symbols, name))
	{
		free(str_temp);
		perr(LABEL_UDEFINED);
		return FALSE;
	}
	
	free(str_temp);
	return TRUE;
}

bool is_valid_register(char *arg)
{
	return get_register(arg) != NOT_REGISTER;
}


/*----------------------private functions----------------------------*/

/*checks if current line has label definition*/
bool is_label(char *line)
{
	char *label_end = strchr(line, ':');
	return (label_end != NULL) && isspace(*(label_end + 1));
}

/*counts number of arguments in args*/
int count_args(char *args)
{
	int count = 1;/*args numm is number of ',' (outside of string) plus 1*/
	bool is_string = FALSE;/*true while reading a string*/

	if (!args)/*no args*/
		return 0;

	while (*args)
	{
		if (*args == ',' && !is_string)/*a comma outside of a string*/
			count++;
		if (*args == '\"')/*starting or ending a string*/
			is_string = !is_string;
		args++;
	}

	return count;
}

/*returns the number of args expected for a given command.
	if the number is unknown (e.g. .array command) or the keyword is illigal returns -1*/
int get_arg_num_expected(command c)
{
	switch (c)
	{
		case MOV: case CMP: case ADD: case SUB: case LEA: case STRUCT_COMMAND:
			return 2;
		case NOT: case CLR:  case INC: case DEC: case JMP: case BNE: case GET:
		case PRN: case JSR: case STRING_COMMAND: case EXTERN_COMMAND: case ENTRY_COMMAND:
			return 1;
		case RTS: case HLT:
			return 0;
		case ARRAY_COMMAND: case NO_COMMAND:
			return -1;
	}
	return -1;
}

/*returns true if name a reserved keyword*/
bool is_keyword(char *name)
{

	return (get_command(name) != NO_COMMAND) &&
		(get_register(name) != NOT_REGISTER) &&
		(!strcmp(name,"macro")) &&
		(!strcmp(name, "endmacro"));
}





/*returns the addresing type of arg.*/
addressing get_addressing(char *arg)
{
	if (!arg) return UNUSED;

	delete_initial_spaces(arg);
	delete_final_spaces(arg);

	if (is_immidiat(arg)) return IMMIDIAT;
	if (is_direct(arg)) return DIRECT;
	if (is_struct(arg)) return STRUCT;
	if (is_register(arg)) return REGISTER;

	perr(LABEL_UDEFINED);
	return ADDRESSING_ILLIGAL;
}

bool is_immidiat(char *arg)
{
	return *arg == '#';
}

bool is_direct(char *arg)
{
	return list_find(symbols, arg) != NULL;
}

bool is_struct(char *arg)
{
	return strchr(arg, '.') != NULL;
}

bool is_register(char *arg)
{
	return *arg == 'r';
}