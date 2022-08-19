
#define SIZE_NAME 30/*max size of lables and macro name (excluding \0)*/

/*creates tags for data stored in nodes*/
typedef enum tag {
	T_MACRO, /*macro*/
	T_ENT,/*entries*/
	T_EXT,/*externals*/
	T_DATA,/*data*/
	T_CODE,/*code*/
	NO_TAG
}tag;

typedef struct node {
	char name[SIZE_NAME + 1];
	void *data;
	tag tag;
	struct node *next;
}node;

typedef struct list {
	node *head;
}list;

node *node_create(char *name, void *data, tag t);
void node_delete(node *n);
list *list_create();
void list_add(list *l, node *n);
void list_delete(list *l);
node *list_find(list *l, char *name);

