#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread-safe-list.h"

void *sum(void *n1, void *n2)
{

	long double *test = malloc(sizeof(long double));; 
	*test = (*((long double *)n1) + *((long double *)n2));

	return test;
}

void *min(void *n1, void *n2)
{
	long double *test = malloc(sizeof(long double));

	*test = (*((long double *)n1) < *((long double *)n2)) ? *((long double *)n1): *((long double *)n2);
	return test;
}

void *multiplyByTwo(void *number)
{
	long double *test = malloc(sizeof(long double));
	*test = ((*(long double *)number) * 2);

	return test;
}

void *multiplyByThreeHundred(void *number)
{
	long double *test = malloc(sizeof(long double));
	*test = ((*(long double *)number) * 300);

	return test;
}

void *shiftChars(void *chara)
{
	char *test = malloc(sizeof(char) * 8);

	for (int i = 0; i < 7; i++)
	{
		test[i] = ((char *)chara)[i] + 1;
	}
	test[7] = '\0';

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
			printf("%s ", ((charNode *)currentNode)->value);

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

	printList(map(testList, multiplyByThreeHundred));

	printf("Reduced : %Lf\n", *((long double *)reduce(testList, sum)));
	printf("Reduced : %Lf\n", *((long double *)reduce(testList, min)));
	deleteList(testList);

	list *secondTestList = createList(TYPE_CHAR);
	char *sa = "abcdefg",
		 *sb = "bbcdefg",
		 *sc = "cbcdefg",
		 *sd = "dbcdefg";
	insert(secondTestList, sa);
	insert(secondTestList, sb);
	insert(secondTestList, sc);
	insert(secondTestList, sd);
	insert(secondTestList, sa);
	insert(secondTestList, sb);

	printList(secondTestList);

	printList(map(secondTestList, shiftChars));

	deleteList(secondTestList);
}