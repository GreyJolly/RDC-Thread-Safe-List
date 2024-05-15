#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread-safe-list.h"
#include "thread_pool.h"

void *multiplyByTwo(void *number)
{
	// TODO: FIX HORRIBLE SOLUTION
	long double *test = malloc(sizeof(long double));
	*test = ((*(long double *)number) * 2);

	return test;
}

void printList(list *list)
{
	baseNode *currentNode = list->head;
	switch (list->listType)
	{
	case TYPE_CHAR:
		printf("{ ");
		while (currentNode != NULL)
		{
			currentNode = currentNode->next;
		}
		printf("}\n");
		break;
	case TYPE_LONGDOUBLE:
		printf("{ ");
		while (currentNode != NULL)
		{
			printf("%Lf ", ((ldoubleNode *)currentNode)->value);
			currentNode = currentNode->next;
		}
		printf("}\n");
		break;
	default:
		printf("Invalid list type\n");
	}
}

int main()
{

	printf("Testing!\n");
	long double a = 1, b = 2, c = 3, d = 4;
	/*
	list *testList = createList(TYPE_LONGDOUBLE);
	insert(testList, &(a));
	insert(testList, &(b));
	insert(testList, &(c));

	removeFromList(testList);
	removeFromList(testList);

	insert(testList, &(a));
	insert(testList, &(b));
	insert(testList, &(c));

	insertAt(testList, 2, &(d));

	removeFromListAt(testList, 0);

	printList(testList);

	printList(map(testList, multiplyByTwo));

	deleteList(testList);

	*/

	threadPool *t = createThreadPool();

	printf("Funzione dalla suite = %d\n", multiplyByTwo);
	fflush(stdout);

	long double *testona0 = malloc(sizeof(long double));
	long double *testona1 = malloc(sizeof(long double));
	long double *testona2 = malloc(sizeof(long double));
	long double *testona3 = malloc(sizeof(long double));
	long double *testona4 = malloc(sizeof(long double));
	long double *testona5 = malloc(sizeof(long double));
	long double *testona6 = malloc(sizeof(long double));
	long double *testona7 = malloc(sizeof(long double));
	long double *testona8 = malloc(sizeof(long double));
	long double *testona9 = malloc(sizeof(long double));

	*testona0 = 0;
	*testona1 = 1;
	*testona2 = 2;
	*testona3 = 3;
	*testona4 = 4;
	*testona5 = 5;
	*testona6 = 6;
	*testona7 = 7;
	*testona8 = 8;
	*testona9 = 9;

	addJob(t, multiplyByTwo, testona0);
	addJob(t, multiplyByTwo, testona1);
	addJob(t, multiplyByTwo, testona2);
	addJob(t, multiplyByTwo, testona3);
	addJob(t, multiplyByTwo, testona4);
	addJob(t, multiplyByTwo, testona5);
	addJob(t, multiplyByTwo, testona6);
	addJob(t, multiplyByTwo, testona7);
	addJob(t, multiplyByTwo, testona8);
	addJob(t, multiplyByTwo, testona9);
	addJob(t, multiplyByTwo, testona1);

	sleep(2);

	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
	printf("RISULTATO AHHHHHHHH = %Lf\n", *((long double *)getFirstResult(t)));
}