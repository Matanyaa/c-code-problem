/*defining all reserved names as enums (excluding registers)*/
typedef enum command {
	/*commands*/
	MOV,
	CMP,
	ADD,
	SUB,
	NOT,
	CLR,
	LEA,
	INC,
	DEC,
	JMP,
	BNE,
	GET,
	PRN,
	JSR,
	RTS,
	HLT,

	/*instructions*/
	ARRAY_COMMAND,
	STRING_COMMAND,
	STRUCT_COMMAND,

	/*extern and entries*/
	EXTERN_COMMAND,
	ENTRY_COMMAND,

	/*no keyword*/
	NO_COMMAND

}command;

typedef enum addressing
{
	IMMIDIAT,
	DIRECT,
	STRUCT,
	REGISTER,
	UNUSED,
	ADDRESSING_ILLIGAL
}addressing;
#define NUM_REGISTERS 8/*number of registers*/
#define NOT_REGISTER 8U
#define REGISTER_NAME_STR_LENGTH 2/*length of register name string (e.g. "r0")*/

/*---------------------------syntax analysis functions---------------------*/
void tokenize_line(char *line, char **label_tok, command *command_tok, char **args_tok);
command get_command(char *word);
unsigned int get_register(char *word);

addressing get_addressing(char *args);

/*-----------------------syntax validation--------------------------*/
bool is_valid_name(char *name, list *l);
bool is_valid_num(char *str);
bool is_valid_command(command command, char *args);
bool is_valid_string(char *str);
bool is_valid_struct(char *str);