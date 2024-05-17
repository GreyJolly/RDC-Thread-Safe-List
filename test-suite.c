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

void insertLongDouble (list * l, int lowerBound, int maxBound, int minValue){
	long double finalValue = 0;
	int value = minValue;
	for (int i = lowerBound; i<maxBound; i++){
		finalValue = (long double)value;
		insert(l, &(finalValue));
		value++;
	}
}

int getAtLongDouble (list * l, int lowerBound, int maxBound, int minValue){
	long double finalValue = 0;
	int value = minValue;
	for (int i = lowerBound; i<maxBound; i++){
		finalValue = (long double)value;
		baseNode * n = getAt(l, maxBound-1 - i);
		if(((ldoubleNode*)n)->value != finalValue){
			return 0;
		}
		value++;
	}
	return 1;
}

int main()
{
	int Test1 = 1, Test2 = 1, Test3 = 1;
	long double gh = 0;

	/*Test 1: creation, insert and GetAt of a series of number in TYPE_LONGDOUBLE*/
	list *LongDoubleList = createList(TYPE_LONGDOUBLE);

	insertLongDouble(LongDoubleList, 0, 10, 0);
	Test1 = getAtLongDouble(LongDoubleList, 0, 10, 0);

	printf("Test 1: %d/1\n", Test1);

	/*TEST 2: remove of the list of new values in the long double list*/

	for (int i = 0; i<10; i++){
		gh = (long double)(i+10);
		baseNode *n = insertAt(LongDoubleList, 0, &gh);
		//printf("VALUE: %Lf GH: %d\n", ((ldoubleNode*)n)->value, i);
		//printList(LongDoubleList);
	}

	for (int i = 0; i<20; i++){
		gh = (long double)i;
		baseNode * n = getAt(LongDoubleList, 19 - i);
		//printf("VALUE: %Lf i: %Lf\n", ((ldoubleNode*)n)->value, gh);
		if(((ldoubleNode*)n)->value != gh){
			Test2=0;
			break;
		}
	}

	printf("Test 2: %d/1\n", Test2);

	/* TEST 3: removeFromList*/
	for (int i = 10; i<20; i++){
		gh = (long double)i;
		insert(LongDoubleList, &(gh));
	}

	for (int i = 0; i<10; i++){
		removeFromList(LongDoubleList);
	}

	for (int i = 0; i<10; i++){
		gh = (long double)i;
		baseNode * n = getAt(LongDoubleList, 9 - i);
		//printf("VALUE: %Lf GH: %Lf\n", ((ldoubleNode*)n)->value, gh);
		if(((ldoubleNode*)n)->value != gh){
			Test3=0;
			break;
		}
	}


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

	printf ("Total pass test: %d/3\n", Test1+Test2+Test3);
}