
#include "include.h"

/*-------------private function declerations-----------*/
bool is_line_blank(FILE *f);
void clear_line(FILE *f);
bool is_end_of_line(char *line);

/*------------file processing functions---------------------------*/

/*opens file name.extension with a given mode and stores its pointer in *f.
	if fails to open prints an error and returns ERR, else returns NO_ERR*/
bool_err open_file(FILE **f, char *name, char *extension, char *mode)
{
	strcat(name, extension);
	*f = fopen(name, mode);
	remove_extension(name);
	return check_fptr(*f);
}

/*deletes a file named file_name.extension*/
void delete_file(char *name, char *extension)
{
	strcat(name, extension);
	remove(name);
	remove_extension(name);
}

/*checks if current file reached EOF*/
bool is_file_empty(FILE *f)
{
	int c = fgetc(f);
	if (c == EOF)
		return TRUE;
	ungetc(c, f);
	return FALSE;
}

/*reads the current line from a given file into line_buffer.*/
/*if the line is too long prints an error, clears the rest of the line and returns ERR.*/
/*else returns NO_ERR*/
bool_err get_line(char *line, FILE *f)
{
	fgets(line, SIZE_LINE + 1, f);

	if(!is_end_of_line(line))/*there are extra chars in the line*/
		if (!is_line_blank(f) && !isspace( *(line + SIZE_LINE)))/*the extra chars are not all white chars or last char in line isn't white space*/
			return LINE_LONG;
	*(line + SIZE_LINE) = '\n';/*replacing the last char in line with \n. this handles the case that line was too long due to white spaces at the end*/
	return NO_ERR;
}

/*this function gets a name of file with its extension (e.g. name.txt).
 it removes anything after and including the '.' character*/
void remove_extension(char *file_name)
{
	*strrchr(file_name, '.') = '\0';
}

/*--------------string manipulations--------------------*/

/*deletes initial white spaces of str*/
void delete_initial_spaces(char *str)
{
	char *ptr;
	ptr = str;

	if(!str)
		return;

	while (isspace(*ptr))
	{
		ptr++;
	}
	memmove(str, ptr, strlen(ptr) + 1);
}

/*deletes final white spaces of str*/
void delete_final_spaces(char *str)
{
	char *ptr;

	if (!str)
		return;

	if (*str == '\0')/*empty string*/
		return;
	ptr = strchr(str, '\0') - 1;

	while (ptr >= str && isspace(*ptr))
	{
		ptr--;
	}
	*(ptr + 1) = '\0';
}

/*creates a dynamicly allocated copy of source and returns a pointer to it*/
char *alloc_str_cpy(char *source)
{
	char *target = malloc((strlen(source) + 1)* sizeof(char));
	check_alloc(target);
	memmove	(target, source, strlen(source) + 1);
	return target;
}

/*---------------private functions------------------------*/

/*returns true if current line has only white chars, else returns false*/
bool is_line_blank(FILE *f)
{
	int c = fgetc(f);
	while (c != EOF && c != (int)'\n' && isspace(c))
		c = fgetc(f);
	if (isspace(c))
		return TRUE;
	ungetc(c, f);
	return FALSE;
}

/*clears the current line in file f*/
void clear_line(FILE *f)
{
	int c = fgetc(f);
	while (c != (int)'\n' && c != EOF)
		c = fgetc(f);
}

/*this function gets the last line that was read and checks that there are no extra chars
	this is done by checking that line contains the '\n' char (returns true), else returns flase*/
bool is_end_of_line(char *line)
{
	if (strchr(line, '\n'))
		return TRUE;

	return FALSE;
}
