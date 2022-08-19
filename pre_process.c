
#include "include.h"

/*-------------private function declerations-----------*/
bool_err pre_process_file(FILE *f_as, FILE *f_am);
bool_err pre_process_line(FILE *f_as, FILE *f_am, char *line, list *macros);

bool is_macro(char *line);
bool is_endmacro(char *line);
char *extract_macro_name(char *line);
bool_err record_macro(FILE *f_as, char *macro_name, list *macros);
node *get_recorded_macro(char *line, list *macros);
/*---------end of private function declerations-----------*/


bool_err pre_process_main(char *file_name)
{
	/*decleration and initialization*/
	FILE *f_as = NULL;
	FILE *f_am = NULL;
	bool_err flag = NO_ERR; /*becomes true(ERR) if encounteres an error in pre_process*/

	/*oppening files and initialization of variables*/
	flag |= open_file(&f_as, file_name, AS, READ);
	flag |= open_file(&f_am, file_name, AM, WRITE);
	line_no = 1;

	if (flag)/*couldnt find file*/
	{
		fclose(f_am);
		delete_file(file_name, AM);/*deleting .am file*/
		return flag;
	}

	flag = pre_process_file(f_as, f_am);/*executed if files opened properly*/

	/*closing files and deleting macro list*/
	fclose(f_as);
	fclose(f_am);

	if (flag)
		delete_file(file_name, AM);/*deleting .am file if pre_proccessing failed*/

	return flag;
}


/*--------------------private functions---------------------*/

/*--------------------procedural functions-------------------*/

/*pre_processing .as file*/
bool_err pre_process_file(FILE *f_as, FILE *f_am)
{
	char line[SIZE_LINE + 1];
	bool_err line_flag = NO_ERR;/*on if current line is too long*/
	bool_err flag = NO_ERR;/*on if encountered an error during the entire pre-processing*/
	list *macros = list_create();
	while (!is_file_empty(f_as))
	{
		line_flag = get_line(line, f_as);
		flag |= line_flag;

		if (!line_flag)
			flag |= pre_process_line(f_as, f_am, line, macros);

		line_no++;
	}

	list_delete(macros);
	return flag;
}

/*processing a line*/
bool_err pre_process_line(FILE *f_as, FILE *f_am, char *line, list *macros)
{
	char *macro_name;
	node *macro = NULL;

	if (is_macro(line))/*starting macro defenition*/
	{
		macro_name = extract_macro_name(line);

		if (!macro_name || (!is_valid_name(macro_name, macros)))/*macro name is null or illigal*/
			return ERR;
		else/*valid macro definition - recording macro*/
			return record_macro(f_as, macro_name, macros);
	}
	else if ((macro = get_recorded_macro(line, macros)))
	{
		fputs((char *)macro->data, f_am);
	}
	else /*line doesn't define a macro*/
	{
		fputs(line, f_am);
	}

	return NO_ERR;
}

/*----------------------macro functions----------------------*/

/*checks if current line is macro definition (first word is macro keyword)*/
bool is_macro(char *line)
{
	char *macro_keyword;

	/*create a copy of original line*/
	char *line_temp = alloc_str_cpy(line);

	macro_keyword = strtok(line_temp, " \t");/*geting first word - must start with macro keyword*/

	if (macro_keyword && !strcmp(macro_keyword, "macro"))
	{
		free(line_temp);
		return TRUE;
	}

	free(line_temp);
	return FALSE;
}

/*returns true if line has endmacro keyword, else returns false*/
bool is_endmacro(char *line)
{
	/*create a copy of original line*/
	char *line_temp = alloc_str_cpy(line);

	delete_initial_spaces(line_temp);
	delete_final_spaces(line_temp);

	if (line_temp && !strcmp(line_temp, "endmacro"))
	{
		free(line_temp);
		return TRUE;
	}

	return FALSE;
}

/*getting macro name from line*/
char *extract_macro_name(char *line)
{
	char *name;

	/*deleting spaces at staart and end of line*/
	delete_initial_spaces(line);
	delete_final_spaces(line);

	/*macro name starts after the keyword 'macro' followed by a single space*/
	strtok(line, " \t");
	name = strtok(NULL, " \t");

	return name;
}

/*recording a macro until next endmacro keyword. if failed returns ERR else returns NO_ERR*/
bool_err record_macro(FILE *f_as, char *macro_name, list *macros)
{
	node *macro;
	char line[SIZE_LINE + 1];
	char *data = malloc(SIZE_MACRO * sizeof(char));

	/*initilization*/
	*line = '\0';/*initilizing line to hold empty string*/
	strcpy(data, "");/*initilizing macro_data to hold empty string*/
	
	while (!is_endmacro(line)) /*recording until endmacro*/
	{
		/*adding last line to macro_data*/
		if (strlen(data) + strlen(line) > SIZE_MACRO)
		{
			perr(MACRO_LONG);
			return ERR;
		}
		strcat(data, line);

		/*getting the next line*/
		line_no++;/*advancing the global line counter*/
		if (get_line(line, f_as))/*recorded line is too long*/
		{
			perr(LINE_LONG);
			return ERR;
		}
	}
	
	macro = node_create(macro_name, data, T_MACRO);
	list_add(macros, macro);
	return NO_ERR;
}

/*gets recorded macro with same name as in line.
	if line doesn't contain a macro name returns null*/
node *get_recorded_macro(char *line, list *macros)
{
	node *n;
	char *name = alloc_str_cpy(line);

	delete_initial_spaces(name);
	delete_final_spaces(name);

	if (name)/*token could be null if line is empty*/
	{
		n = list_find(macros, name);
		free(name);
		return n;
	}

	return NULL;
}
