#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "thread-safe-list.h"

static void handleError(char *error, char perrorFlag)
{

	if (perrorFlag)
		perror(error);
	else
		printf("%s\n", error);
	exit(EXIT_FAILURE);
}

list *createList(char type)
{
	// TODO: check inputs

	list *newList = (list *)malloc(sizeof(list));
	newList->head = NULL;
	newList->tail = NULL;
	newList->lastAccess = NULL;
	newList->listType = type;

	int ret = sem_init(&(newList->accessing), 0, 1);
	if (ret)
		handleError("Couldn't create semaphore", 1);



	return newList;
}

void deleteList(list *l)
{
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
	free(l);

	ret = sem_post(&(l->accessing));
	if (ret)
		handleError("sem_post", 1);
}

baseNode *getAt(list *l, int index)
{
	// TODO: check inputs
	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("getAt: invalid List", 0);

	baseNode *currentNode = l->head;
	for (int i = 0; i < index; i++)
	{
		if (currentNode == NULL)
			return NULL; // returns NULL if list is too short
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
	// TODO: check inputs
	baseNode *newNode;

	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insert: invalid List", 0);

	switch (l->listType)
	{
	case TYPE_CHAR:
		newNode = malloc(sizeof(charNode));
		strcpy(((charNode *)newNode)->value, value);
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

baseNode *insertAt(list *l, int index, void *value)
{
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
		strcpy(((charNode *)newNode)->value, value);
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
			return NULL; // returns NULL if the list is too short
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
	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insertAt: invalid List", 0);
	if (l->head == NULL)
		return NULL;

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
baseNode *removeFromListAt(list *l, int index)
{
	if (index == 0)
		// Handle case where we're removing head
		return removeFromList(l);
	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insertAt: invalid List", 0);
	if (l->head == NULL)
		return NULL;

	baseNode *freeableNode = l->head, *returnNode;

	for (int i = 0; i < index; i++)
	{
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
	int ret = sem_wait(&(l->accessing));
	if (ret)
		handleError("sem_wait", 1);

	if (l == NULL)
		handleError("insertAt: invalid List", 0);

	list *newList = createList(l->listType);
	baseNode *currentNode = l->tail;

	switch (l->listType)
	{
	case TYPE_CHAR:
		while (currentNode != NULL)
		{
			insert(newList, function(&((ldoubleNode *)currentNode)->value));
			currentNode = currentNode->prev;
		}
		break;
	case TYPE_LONGDOUBLE:
		while (currentNode != NULL)
		{
			insert(newList, function(&((charNode *)currentNode)->value));
			currentNode = currentNode->prev;
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
