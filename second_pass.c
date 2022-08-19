
#include "include.h"

/*defining shifts for base conversion and calculations*/
#define SHIFT_NEXT_SYMBOL 5/*number of bits to be shifted to get the next 32 base symbol*/
#define SHIFT_OP 6/*number of bits to shift opcode bits*/
#define SHIFT_SOURCE_ADDRESS 4/*number of bits to shift source addressing bits*/
#define SHIFT_TARGET_ADDRESS 2/*number of bits to shift target addressing bits*/
#define SHIFT_IMMIDIATE 2/*number of bits to shift immidiate/adress numbers*/
#define SHIFT_SORCE_REGISTER 6/*number of bits to shift source register bits*/
#define SHIFT_TARGET_REGISTER 2/*number of bits to shift target register bits*/
#define UNUSED_REGISTER 0

/*define A,E,R*/
#define A 0U
#define E 1U
#define R 2U

#define MASK_5 31U/*only fisrt 5 lsb bits are on (0000011111)*/
/*-------------private function declerations-----------*/
bool_err second_pass_file(FILE *f_am, FILE *f_ob, FILE *f_ext, FILE *f_ent);
bool_err second_pass_line(char *line, FILE *f_ob, FILE *f_ext, FILE *f_ent);

bool_err write_line(command c, char *args, FILE *f_ob, FILE *f_ext, FILE *f_ent);
bool_err translate_code_line(command c, char *source, char *target, FILE *f_ob, FILE *f_ext);
bool_err write_arg(FILE *f_ob, FILE *f_ext, char *arg, addressing a, bool is_register_source);
bool is_addressing_legal(command c, addressing source, addressing target);

void write_ob_header(FILE *f_ob);
void write_word(FILE *f, unsigned int word);
bool_err write_entry(char *arg, FILE *f_ent);
bool_err write_command(FILE *f_ob, command c, addressing addres1, addressing addres2);
bool_err write_immidiate(FILE *f_ob, char *arg);
bool_err write_direct(FILE *f_ob, FILE *f_ext, char *arg);
bool_err write_struct(FILE *f_ob, FILE *f_ext, char *arg);
bool_err write_register(FILE *f_ob, unsigned int source, unsigned int target);
void write_data_image(FILE *f_ob);

void num_to_32(char *str, unsigned int num);
char num_to_32_tabel(unsigned int num);

/*---------end of private function declerations-----------*/

bool_err second_pass_main(char *file_name)
{
	/*decleration and initialization*/
	FILE *f_am = NULL;
	FILE *f_ob = NULL;
	FILE *f_ext = NULL;
	FILE *f_ent = NULL;
	bool_err flag = NO_ERR; /*becomes true(ERR) if encounteres an error in pre_process*/

	/*oppening files and initialization of variables*/
	flag |= open_file(&f_am, file_name, AM, READ);
	flag |= open_file(&f_ob, file_name, OB, WRITE);
	flag |= open_file(&f_ext, file_name, EXT, WRITE);
	flag |= open_file(&f_ent, file_name, ENT, WRITE);
	line_no = 1;

	if (!flag)
		flag = second_pass_file(f_am, f_ob, f_ext, f_ent);/*executed if files opened properly*/


	/*closing files*/
	fclose(f_am);
	fclose(f_ob);
	fclose(f_ext);
	fclose(f_ent);

	if (flag)
		delete_file(file_name, OB);/*deleting .ob file if second_pass failed*/
	if (!is_file_empty(f_ext))
		delete_file(file_name, EXT);/*deleting .ext file if there are no extern symbols*/
	if (!is_file_empty(f_ent))
		delete_file(file_name, ENT);/*deleting .ent file if there are no entry symbols*/

	return flag;
}


/*--------------------procedural functions-------------------*/

/*second pass of .am file*/
bool_err second_pass_file(FILE *f_am, FILE *f_ob, FILE *f_ext, FILE *f_ent)
{
	char line[SIZE_LINE + 1];
	bool_err line_flag = NO_ERR;/*on if failed to read line*/
	bool_err flag_pass = NO_ERR;/*on if encountered an error during pre-processing*/

	write_ob_header(f_ob);
	IC = START_ADRESS;

	while (!is_file_empty(f_am))
	{
		line_flag = get_line(line, f_am);
		flag_pass |= line_flag;

		if (!line_flag)
		{
			flag_pass |= second_pass_line(line, f_ob, f_ext, f_ent);
		}
		

		line_no++;
	}
	write_data_image(f_ob);

	return flag_pass;
}

/*second pass of a single line*/
bool_err second_pass_line(char *line, FILE *f_ob, FILE *f_ext, FILE *f_ent)
{
	/*tokens declerations*/
	char *label_tok = NULL;/*in this stage this is ignored*/
	char *args_tok = NULL;
	command command_tok;
	bool_err flag = NO_ERR;

	
	if (*line == '\n' || *line == '\r' || *line == ';')/*line is empty or a comment*/
	{
		line_no++;
		return NO_ERR;
	}
	tokenize_line(line, &label_tok, &command_tok, &args_tok);
	flag |= write_line(command_tok, args_tok, f_ob, f_ext, f_ent);

	return flag;
}

/*----------------------support functions----------------------*/
/*prints the ob header - IC ID in required base*/
void write_ob_header(FILE *f_ob)
{
	char IC32[NUM_DIGITS];
	char DC32[NUM_DIGITS];

	num_to_32(IC32, (unsigned int)IC - START_ADRESS);
	num_to_32(DC32, (unsigned int)DC);

	fprintf(f_ob, "%s %s\n", IC32, DC32);
}

/*write entire line in base 32*/
bool_err write_line(command c, char *args, FILE *f_ob, FILE *f_ext, FILE *f_ent)
{
	bool_err flag = NO_ERR;
	char *source = NULL;
	char *target = NULL;

	switch (c)
	{
		case MOV: case CMP: case ADD: case SUB: case LEA: 
			source = strtok(args, ",");
			target = strtok(NULL, "");
			break;

		case NOT: case CLR:  case INC: case DEC: case JMP: case PRN: case JSR: case BNE: case GET: 
			source = NULL;
			target = args;
			break;

		case RTS: case HLT:
			source = NULL;
			target = NULL;
			break;

		case ENTRY_COMMAND:
			flag |= write_entry(args, f_ent);
			return flag;

		default:/*does nothing on other cases*/
			return flag;
	}
	flag |= translate_code_line(c, source, target, f_ob, f_ext);

	return flag;
}

/*translate a line containing code instruction (e.g. mov, add...)*/
bool_err translate_code_line(command c, char *source, char *target, FILE *f_ob, FILE *f_ext)
{
	bool_err flag = NO_ERR;
	addressing source_address = get_addressing(source);
	addressing target_address = get_addressing(target);

	if (!is_addressing_legal(c, source_address, target_address))
	{
		perr(ADDRESSING_ERROR);
		return ERR;
	}

	flag |= write_command(f_ob, c, source_address, target_address);

	if (source_address == REGISTER && target_address == REGISTER)
		write_register(f_ob, get_register(source), get_register(target));
	else
	{
		write_arg(f_ob, f_ext, source, source_address, TRUE);
		write_arg(f_ob, f_ext, target, target_address, FALSE);
	}

	return NO_ERR;
}

bool is_addressing_legal(command c, addressing source, addressing target)
{
	if (source == ADDRESSING_ILLIGAL || target == ADDRESSING_ILLIGAL)
		return FALSE;

	switch (c)
	{
		case MOV: case ADD: case SUB: case NOT: case CLR: case INC: case DEC: case JMP: case BNE: case GET: case JSR:
			return target != IMMIDIAT;
		case LEA:
			return (target != IMMIDIAT) && (source != IMMIDIAT || source != REGISTER);
		default:
			;
	}

	return TRUE;
}

/*-------------------write functions - functions that write to .ob file----------------------*/

void write_word(FILE *f, unsigned int word)
{
	char word_32[NUM_DIGITS];
	char IC_32[NUM_DIGITS];
	num_to_32(word_32, word);
	num_to_32(IC_32, IC);
	fprintf(f, "%s\t%s\n", IC_32, word_32);
	IC++;
}

bool_err write_arg(FILE *f_ob, FILE *f_ext, char *arg, addressing a, bool is_register_source)
{
	/*unused argument*/
	if (!arg)
		return NO_ERR;

	delete_initial_spaces(arg);
	delete_final_spaces(arg);

	switch (a)
	{
		case IMMIDIAT:
			return write_immidiate(f_ob, arg);

		case DIRECT:
			return write_direct(f_ob, f_ext, arg);

		case STRUCT:
			return write_struct(f_ob, f_ext, arg);

		case REGISTER:
			if(is_register_source)
				return write_register(f_ob, get_register(arg), UNUSED_REGISTER);
			else
				return write_register(f_ob, UNUSED_REGISTER, get_register(arg));

		default:
			return ERR;
	}
}

/*write entry symbol into .ent file*/
bool_err write_entry(char *arg, FILE *f_ent)
{
	char value[NUM_DIGITS];
	node *n = list_find(symbols, arg);
	unsigned int word;

	if (!n)
	{
		perr(ENTRY_UDEFINED);
		return ERR;
	}

	word = *(unsigned int *)(n->data);
	num_to_32(value,word);

	fprintf(f_ent, "%s\t%s\n", n->name, value);

	return NO_ERR;
}

bool_err write_command(FILE *f_ob, command c, addressing source, addressing target)
{
	unsigned int word;
	unsigned int command_word = (unsigned int)c;
	unsigned int source_word = (unsigned int)source;
	unsigned int target_word = (unsigned int)target;

	if (source == UNUSED)
		source_word = 0U;
	if (target == UNUSED)
		target_word = 0U;

	word = (command_word << SHIFT_OP);
	word |= (source_word << SHIFT_SOURCE_ADDRESS);
	word |= (target_word << SHIFT_TARGET_ADDRESS);
	write_word(f_ob, word);

	return NO_ERR;
}

bool_err write_immidiate(FILE *f_ob, char *arg)
{
	unsigned int word;

	if (is_valid_num(arg))
	{
		arg = strtok(arg, "#");
		word = (unsigned int)atoi(arg);
		word <<= SHIFT_IMMIDIATE;
		write_word(f_ob, word);
		return NO_ERR;
	}

	return ERR;
}

bool_err write_direct(FILE *f_ob, FILE *f_ext, char *arg)
{
	unsigned int word;
	node *n;

	if ((n = list_find(symbols, arg)))
	{
		if (n->tag == T_EXT)
		{
			char value[NUM_DIGITS];

			word = E;

			num_to_32(value, IC);
			fprintf(f_ext, "%s\t%s\n", n->name, value);
		}
		else
		{
			word = *(unsigned int *)(n->data);
			word <<= SHIFT_IMMIDIATE;
			word += R;
		}
		write_word(f_ob, word);
		return NO_ERR;
	}

	perr(LABEL_UDEFINED);
	return ERR;
}

bool_err write_struct(FILE *f_ob, FILE *f_ext, char *arg)
{
	char *word1;
	char *word2;
	char *arg_temp = alloc_str_cpy(arg);

	if (!is_valid_struct(arg_temp))
	{
		free(arg_temp);
		return ERR;
	}

	word1 = strtok(arg_temp, ".");
	word2 = strtok(NULL, ".");

	write_direct(f_ob, f_ext, word1);
	write_immidiate(f_ob, word2);

	free(arg_temp);
	return NO_ERR;
}

bool_err write_register(FILE *f_ob, unsigned int source, unsigned int target)
{
	unsigned int word;

	source <<= SHIFT_SORCE_REGISTER;
	target <<= SHIFT_TARGET_REGISTER;
	word = source | target;

	write_word(f_ob, word);

	return NO_ERR;
}

void write_data_image(FILE *f_ob)
{
	int i;

	for (i = 0; i < num_ocupide_data_cells; i++)
	{
		write_word(f_ob, data_image[i]);
	}
}



/*-------------------base conversions---------------------*/

/*converting a 10-bit number to base 32 and storing the resault in str*/
void num_to_32(char *str, unsigned int num)
{
	int i;
	char num32[NUM_DIGITS];

	num32[NUM_DIGITS - 1] = '\0';
	for (i = NUM_DIGITS - 2; i >= 0; i--)
	{
		num32[i] = num_to_32_tabel(num & MASK_5);
		num >>= SHIFT_NEXT_SYMBOL;
	}

	strcpy(str, num32);
}

/*converting a 5-bit number to base 32*/
char num_to_32_tabel(unsigned int num)
{
	switch (num)
	{
		case 0: return '!';
		case 1: return '@';
		case 2: return '#';
		case 3: return '$';
		case 4: return '%';
		case 5: return '^';
		case 6: return '&';
		case 7: return '*';
		case 8: return '<';
		case 9: return '>';
		case 10: return 'a';
		case 11: return 'b';
		case 12: return 'c';
		case 13: return 'd';
		case 14: return 'e';
		case 15: return 'f';
		case 16: return 'g';
		case 17: return 'h';
		case 18: return 'i';
		case 19: return 'j';
		case 20: return 'k';
		case 21: return 'l';
		case 22: return 'm';
		case 23: return 'n';
		case 24: return 'o';
		case 25: return 'p';
		case 26: return 'q';
		case 27: return 'r';
		case 28: return 's';
		case 29: return 't';
		case 30: return 'u';
		case 31: return 'v';
	}
	return '~';/*err charachter*/
}