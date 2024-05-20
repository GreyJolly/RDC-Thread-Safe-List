#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "thread-safe-list.h"

typedef struct toBeReduced
{
	int solved;
	void *value;
	unsigned int ticket;
	struct toBeReduced *next;
} toBeReduced;

// Structure to handle the arguments for the thread Pool
typedef struct
{
	void *(*function)(void *, void *);
	void *arg1;
	void *arg2;
} args;

// List used to retire jobs from thread pool
typedef struct ticketList
{
	int ticket;
	struct ticketList *next;
} ticketList;

static void handleError(char *error, char perrorFlag)
{

	if (perrorFlag)
		perror(error);
	else
	{
		printf("%s\n", error);
		errno = EINVAL;
	}
	exit(EXIT_FAILURE);
}

list *createList(unsigned char type)
{
	switch (type)
	{
	case TYPE_CHAR:
	case TYPE_LONGDOUBLE:
		break;
	default:
		errno = EINVAL;
		return NULL;
	}

	list *newList = (list *)malloc(sizeof(list));
	newList->head = NULL;
	newList->tail = NULL;
	newList->lastAccess = NULL;
	newList->listType = type;
	newList->pool = createThreadPool();

	int ret = sem_init(&(newList->accessing), 0, 1);
	if (ret)
		handleError("Couldn't create semaphore", 1);

	return newList;
}

void deleteList(list *l)
{
	if (l == NULL)
		return;
	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	baseNode *currentNode = l->head, *freeableNode;
	while (currentNode != NULL)
	{
		freeableNode = currentNode;
		currentNode = currentNode->next;
		free(freeableNode);
	}

	ret = sem_destroy(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	deleteThreadPool(l->pool);
	free(l);
}

baseNode *getAt(list *l, unsigned int index)
{
	if (l == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("getAt: invalid List", 0);

	baseNode *currentNode = l->head;
	for (int i = 0; i < index; i++)
	{
		if (currentNode == NULL)
		{
			ret = sem_post(&(l->accessing));
			if (ret)
				handleError("sem_post", 1);
			return NULL; // returns NULL if list is too short
		}
		currentNode = currentNode->next;
	}

	l->lastAccess = currentNode;

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);

	return currentNode;
}

baseNode *insert(list *l, void *value)
{
	if (l == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	baseNode *newNode;

	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	switch (l->listType)
	{
	case TYPE_CHAR:
		newNode = malloc(sizeof(charNode));
		memcpy(((charNode *)newNode)->value, value, TYPE_CHAR_LENGTH);
		break;
	case TYPE_LONGDOUBLE:
		newNode = malloc(sizeof(ldoubleNode));
		((ldoubleNode *)newNode)->value = *(long double *)value;

		break;
	default:
		handleError("Invalid list type", 0);
	}

	newNode->next = l->head;
	newNode->prev = NULL;
	if (l->head != NULL)
		l->head->prev = newNode;
	else
		// Handle case where the list doesn't have any nodes yet:
		l->tail = newNode;
	l->head = newNode;

	l->lastAccess = newNode;

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);

	return newNode;
}

baseNode *insertAt(list *l, unsigned int index, void *value)
{
	if (l == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	if (index == 0)
		return insert(l, value);
	baseNode *newNode, *currentNode;

	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insertAt: invalid List", 0);
	currentNode = l->head;

	switch (l->listType)
	{
	case TYPE_CHAR:
		newNode = malloc(sizeof(charNode));
		memcpy(((charNode *)newNode)->value, value, TYPE_CHAR_LENGTH);
		break;
	case TYPE_LONGDOUBLE:
		newNode = malloc(sizeof(ldoubleNode));
		((ldoubleNode *)newNode)->value = *(long double *)value;
		break;
	default:
		handleError("Invalid list type", 0);
	}

	for (int i = 0; i < index; i++)
	{
		if (currentNode == NULL)
		{
			ret = sem_post(&(l->accessing));
			if (ret)
				handleError("sem_post", 1);
			return NULL; // returns NULL if the list is too short
		}

		currentNode = currentNode->next;
	}

	newNode->next = currentNode;
	if (currentNode != NULL)
	{
		// Handle adding anywhere but on the tail:
		newNode->prev = currentNode->prev;
		// Handle adding in head:
		if (currentNode->prev != NULL)
			currentNode->prev->next = newNode;
		currentNode->prev = newNode;
	}
	else
	{
		// Handle adding to tail:
		newNode->prev = l->tail;
		l->tail->next = newNode;
		l->tail = newNode;
	}

	l->lastAccess = newNode;

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);

	return newNode;
}

baseNode *removeFromList(list *l)
{
	if (l == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insertAt: invalid List", 0);
	if (l->head == NULL)
	{
		ret = sem_post(&(l->accessing));
		if (ret)
			handleError("sem_post", 1);
		return NULL;
	}

	baseNode *freeableNode = l->head;

	if (freeableNode->next != NULL)
	{
		// Handle every case where we're not removing the last element
		l->head = freeableNode->next;
		l->head->prev = NULL;
	}
	else
	{
		// Handle case where we're removing the last element
		l->head = NULL;
	}

	if (l->lastAccess == freeableNode)
		l->lastAccess = NULL;

	free(freeableNode);

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);

	return l->head;
}
baseNode *removeFromListAt(list *l, unsigned int index)
{
	if (l == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	if (index == 0)
		// Handle case where we're removing head
		return removeFromList(l);
	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insertAt: invalid List", 0);
	if (l->head == NULL)
	{
		ret = sem_post(&(l->accessing));
		if (ret)
			handleError("sem_post", 1);
		return NULL;
	}

	baseNode *freeableNode = l->head, *returnNode;

	for (int i = 0; i < index; i++)
	{
		if (freeableNode->next == NULL)
		{
			ret = sem_post(&(l->accessing));
			if (ret)
				handleError("sem_post", 1);
			return NULL;
		}
		freeableNode = freeableNode->next;
	}

	if (freeableNode->next != NULL)
	{
		// Handle every case where we're not removing the last element
		freeableNode->next->prev = freeableNode->prev;
		freeableNode->prev->next = freeableNode->next;
	}
	else
	{
		// Handle case where we're removing the last element
		l->tail = freeableNode->prev;
		l->tail->next = NULL;
	}

	if (l->lastAccess == freeableNode)
		l->lastAccess = NULL;

	returnNode = freeableNode->next;
	free(freeableNode);

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);

	return returnNode;
}

list *map(list *l, void *(*function)(void *))
{
	if (l == NULL || function == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insertAt: invalid List", 0);

	list *newList = createList(l->listType);
	baseNode *currentNode = l->head;
	ticketList *head = NULL, *node = NULL;

	switch (l->listType)
	{
	case TYPE_CHAR:
		while (currentNode != NULL)
		{
			node = malloc(sizeof(ticketList));
			node->ticket = addJob(l->pool, function, ((charNode *)currentNode)->value);
			node->next = head;
			head = node;
			currentNode = currentNode->next;
		}
		while (head != NULL)
		{
			insert(newList, getResult(l->pool, head->ticket));
			node = head;
			head = head->next;
			free(node);
		}
		break;
	case TYPE_LONGDOUBLE:
		while (currentNode != NULL)
		{
			node = malloc(sizeof(ticketList));
			node->ticket = addJob(l->pool, function, (&((ldoubleNode *)currentNode)->value));
			node->next = head;
			head = node;
			currentNode = currentNode->next;
		}
		while (head != NULL)
		{
			insert(newList, getResult(l->pool, head->ticket));
			node = head;
			head = head->next;
			free(node);
		}

		break;
	default:
		handleError("Invalid list type", 0);
	}

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);

	return newList;
}

void *poolReadyFunction(void *arg)
{
	// The thread pool only accepts functions of type void *(*function)(void *, void *), here we're converting them
	return ((args *)arg)->function(((args *)arg)->arg1, ((args *)arg)->arg2);
}

void *reduce(list *l, void *(*function)(void *, void *))
{
	if (l == NULL || function == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l->head == NULL)
	{
		ret = sem_post(&(l->accessing));
		if (ret)
			handleError("sem_post", 1);
		return NULL;
	}

	baseNode *toBeAdded = l->head;									  // Node to iterate on the list
	toBeReduced *head = malloc(sizeof(toBeReduced)), *tail, *newHead; // Nodes of the new list that handles the calls

	// Setup initial head and tail (we already know l->head != NULL)
	switch (l->listType)
	{
	case (TYPE_CHAR):
		head->value = ((charNode *)toBeAdded)->value;
		break;

	case (TYPE_LONGDOUBLE):
		head->value = &(((ldoubleNode *)toBeAdded)->value);
		break;
	default:
		handleError("Invalid list type", 0);
	}
	head->next = NULL;
	head->solved = 1;
	head->ticket = 255;
	tail = head;
	toBeAdded = toBeAdded->next;

	// Setup all other nodes
	while (toBeAdded != NULL)
	{
		newHead = malloc(sizeof(toBeReduced));
		newHead->value = &(((ldoubleNode *)toBeAdded)->value);
		newHead->next = head;
		newHead->solved = 1;
		newHead->ticket = 255;
		head = newHead;

		toBeAdded = toBeAdded->next;
	}

	// Iterate on the new list
	while (head->next != NULL)
	{
		toBeReduced *newNode = malloc(sizeof(toBeReduced)), *freeableNode = head;

		// If the node is already solved, use the result, else get it from the pool
		args *arg = malloc(sizeof(args));
		arg->function = function;
		arg->arg1 = (head->solved) ? head->value : getResult(l->pool, head->ticket);
		arg->arg2 = (head->next->solved) ? head->next->value : getResult(l->pool, head->next->ticket);

		newNode->ticket = addJob(l->pool, poolReadyFunction, arg);
		newNode->solved = 0;
		newNode->value = 0;

		newNode->next = NULL;

		// Update the list
		head = freeableNode->next->next;

		free(freeableNode->next);
		free(freeableNode);

		// Update the tail only if those weren't the last two elements of the list, otherwise update the head
		if (head != NULL)
		{
			tail->next = newNode;
			tail = newNode;
		}
		else
		{
			head = newNode;
		}
	}
	void *result = (head->solved) ? head->value : getResult(l->pool, head->ticket);

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);

	return result;
}
