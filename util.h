
/*defining file extensions*/
#define AS ".as"
#define AM ".am"
#define OB ".ob"
#define ENT ".ent"
#define EXT ".ext"

/*defining file open modes*/
#define READ "r"
#define WRITE "w"

/*------------file processing functions---------------------------*/
bool_err open_file(FILE **f, char *name, char *extension, char *mode);
void delete_file(char *name, char *extension);
bool is_file_empty(FILE *f);
bool_err get_line(char *line_buffer, FILE *f);
void remove_extension(char *file_name);

/*--------------string manipulations--------------------*/
void delete_initial_spaces(char *str);
void delete_final_spaces(char *str);
char *alloc_str_cpy(char *source);
