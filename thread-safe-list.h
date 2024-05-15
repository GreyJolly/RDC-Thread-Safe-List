#include <semaphore.h>

enum listTypes
{
	TYPE_CHAR,
	TYPE_LONGDOUBLE
};

// Base node:
typedef struct baseNode
{
	struct baseNode *prev;
	struct baseNode *next;
} baseNode;

// Derived nodes (base node is first so that casting is easier):
typedef struct
{
	baseNode base;
	unsigned char value[8];
} charNode;

typedef struct
{
	baseNode base;
	long double value;
} ldoubleNode;

typedef struct
{
	char listType;
	baseNode *head;
	baseNode *tail;
	baseNode *lastAccess;
	sem_t accessing;
} list;

list *createList(char type);
void deleteList(list *l);
baseNode *getAt(list *l, int index);
baseNode *insert(list *l, void *value);
baseNode *insertAt(list *l, int index, void *value);
baseNode *removeFromList(list *l);
baseNode *removeFromListAt(list *l, int index);

list *map(list *l, void *(*function)(void *));
