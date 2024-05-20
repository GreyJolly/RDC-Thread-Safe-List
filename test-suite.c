#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "thread-safe-list.h"
#include <pthread.h>

#define NUMBER_OF_TESTS 50
#define NUMBER_THREAD 10

typedef struct argThreads
{
	list *l;
	void *value;
	int functionID;
	int index;
	int ret;
	void* (*function)(void*, void*);

} argThreads;

enum ThreadsFunctions
{
	INSERT_THREADS,
	GETAT_THREADS,
	INSERTAT_THREADS,
	REMOVEFROMLIST_THREADS,
	REMOVEFROMLISTAT_THREADS,
	MAP_THREADS,
	REDUCE_THREADS
};

long double *sum(long double *n1, long double *n2)
{
	long double *test = malloc(sizeof(long double));
	*test = *n1 + *n2;

	return test;
}

long double *min(long double *n1, long double *n2)
{
	long double *test = malloc(sizeof(long double));

	*test = *n1 < *n2 ? *n1 : *n2;
	return test;
}

long double *multiplyByTwo(long double *number)
{
	long double *test = malloc(sizeof(long double));
	*test = (*number) * 2;

	return test;
}

char *shiftChars(char *chara)
{
	char *test = malloc(sizeof(char) * 8);

	for (int i = 0; i < 7; i++)
	{
		test[i] = chara[i] + 1;
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

int insertLongDouble(list *l, int lowerBound, int upperBound, int minValue)
{
	long double finalValue = 0;
	int value = minValue;
	for (int i = lowerBound; i < upperBound; i++)
	{
		finalValue = (long double)value;
		baseNode *n = insert(l, &(finalValue));
		if (n == NULL && errno == EINVAL)
			return -1;
		value++;
	}
	return 1;
}

int insertChar(list *l, int lowerBound, int upperBound, char startingChar)
{
	char value[8] = {startingChar, startingChar + 1, startingChar + 2, startingChar + 3, startingChar + 4, startingChar + 5, startingChar + 6, startingChar + 7};
	for (int i = lowerBound; i < upperBound; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			value[j] = startingChar + j;
		}
		baseNode *n = insert(l, value);
		if (n == NULL && errno == EINVAL)
			return -1;
		startingChar++;
	}
	return 1;
}

int getAtLongDouble(list *l, int lowerBound, int upperBound, int minValue)
{
	long double finalValue = 0;
	int value = minValue;
	for (int i = lowerBound; i < upperBound; i++)
	{
		finalValue = (long double)value;
		baseNode *n = getAt(l, upperBound - 1 - i);
		if (n == NULL && errno == EINVAL)
			return -1;
		if (((ldoubleNode *)n)->value != finalValue)
		{
			return 0;
		}

		value++;
	}
	return 1;
}

int getAtChar(list *l, int lowerBound, int upperBound, char startingChar)
{
	char value[8] = {startingChar, startingChar + 1, startingChar + 2, startingChar + 3, startingChar + 4, startingChar + 5, startingChar + 6, startingChar + 7};
	for (int i = lowerBound; i < upperBound; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			value[j] = startingChar + j;
		}
		baseNode *n = getAt(l, upperBound - 1 - i);
		if (n == NULL && errno == EINVAL)
			return -1;
		if (memcmp(((charNode *)n)->value, value, 8) != 0)
			return 0;
		startingChar++;
	}
	return 1;
}

int compareList(list *l1, list *l2)
{
	int index = 0;
	baseNode *n1 = getAt(l1, index);
	baseNode *n2 = getAt(l2, index);
	while (n1 != NULL && n2 != NULL)
	{
		if (n1 == NULL && errno == EINVAL)
			return -1;
		if (n2 == NULL && errno == EINVAL)
			return -1;
		if (((ldoubleNode *)n1)->value != ((ldoubleNode *)n2)->value)
			return 0;
		index++;
		n1 = getAt(l1, index);
		n2 = getAt(l2, index);
	}

	return 1;
}

int numElements(list *l1, list *l2)
{
	int index1 = 0, index2 = 0;
	baseNode *n1 = getAt(l1, index1);
	while (n1 != NULL)
	{
		if (n1 == NULL && errno == EINVAL)
			return -1;
		index1++;
		n1 = getAt(l1, index1);
	}
	baseNode *n2 = getAt(l2, index2);
	while (n2 != NULL)
	{
		if (n2 == NULL && errno == EINVAL)
			return -1;
		index2++;
		n2 = getAt(l2, index2);
	}
	return (index1 == index2);
}

void *testThreadFunction(struct argThreads *argv)
{
	switch (argv->functionID)
	{
	case INSERT_THREADS:
		baseNode *n = insert(argv->l, argv->value);
		if (errno == EINVAL && n == NULL)
		{
			argv->ret = 0;
		}
		free(argv);
		break;
	case GETAT_THREADS:
		n = getAt(argv->l, argv->index);
		if (errno == EINVAL && n == NULL)
		{
			argv->ret = 0;
		}
		if ((long double)(NUMBER_THREAD-1-argv->index) != ((ldoubleNode *)n)->value)
		{
			argv->ret = 0;
		}
		break;

	case REDUCE_THREADS:
		long double * r = ((long double *)reduce(argv->l, (void *)(void *)argv->function));
		argv->ret= *(long double*)(argv->value) == *r;
		break;
	}
}

int main()
{
	printf("Started testing...\n");
	int Test[NUMBER_OF_TESTS];
	for (int i = 0; i < NUMBER_OF_TESTS; i++)
	{
		Test[i] = 0;
	}
	int index = 0;

	long double gh = 0;

	// Start timing tests
	clock_t begin = clock();

	/*Creation of a List with invalid type*/
	list *l1 = createList(NUMBER_THREAD);
	Test[index] = errno == EINVAL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Invalid remove from list*/
	baseNode *n = removeFromList(l1);
	Test[index] = errno == EINVAL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Creation, insert and GetAt of a series of number in TYPE_CHAR*/
	l1 = createList(TYPE_CHAR);
	Test[index] = insertChar(l1, 0, NUMBER_THREAD, 'a');
	if (Test[index] != -1)
		Test[index] = getAtChar(l1, 0, NUMBER_THREAD, 'a');

	printf("Test %2d:\t%d/1\n", ++index, Test[index]);
	deleteList(l1);

	/*Creation, insert and GetAt of a series of number in TYPE_LONGDOUBLE*/
	l1 = createList(TYPE_LONGDOUBLE);
	Test[index] = insertLongDouble(l1, 0, NUMBER_THREAD, 0);
	if (Test[index] != -1)
		Test[index] = getAtLongDouble(l1, 0, NUMBER_THREAD, 0);
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Error GetAt in a list*/
	n = getAt(l1, 10000);
	Test[index] = errno == EINVAL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Error in the insert At*/
	gh = (long double)1;
	n = insertAt(l1, 10000, &gh);
	Test[index] = errno == EINVAL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*insertAt of element*/
	gh = (long double)10;
	n = insertAt(l1, 0, &gh);
	Test[index] = getAtLongDouble(l1, 0, 11, 0);
	if (Test[index] == -1)
		Test[index] = 0;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*removeAt of element*/
	n = removeFromListAt(l1, 0);
	Test[index] = getAtLongDouble(l1, 0, 10, 0);
	if (Test[index] == -1)
		Test[index] = 0;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Error Remove At of an element*/
	n = removeFromListAt(l1, 10000);
	Test[index] = errno == EINVAL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Error in map, list inavlid*/
	list *l2 = map(NULL, (void *)(void *)multiplyByTwo);
	Test[index] = errno == EINVAL && l2 == NULL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Error in map, function invalid */
	l2 = map(l1, NULL);
	Test[index] = errno == EINVAL && l2 == NULL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Map*/
	list *DoubleList = createList(TYPE_LONGDOUBLE);
	for (int i = 0; i < 10; i++)
	{
		gh = (long double)(i * 2);
		baseNode *node = insert(DoubleList, &gh);
	}

	l2 = map(l1, (void *)(void *)multiplyByTwo);
	if (l2 == NULL && errno == EINVAL)
		Test[index] = 0;
	Test[index] = compareList(l2, DoubleList);
	printf("Test %d:\t%d/1\n", ++index, Test[index]);

	/*Error in reduce: list invalid*/
	long double * r = ((long double *)reduce(NULL, (void *)(void *)sum));
	Test[index] = errno == EINVAL && r == NULL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*Error in reduce: function invalid*/
	 r = ((long double *)reduce(l1, NULL));
	Test[index] = errno == EINVAL && r == NULL;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);

	/*reduce*/
	long double tot = 0;
	for(int i = 0; i<NUMBER_THREAD;i++){
		baseNode* n = getAt(l1,i);
		tot += ((ldoubleNode *)n)->value;
	}
	r = ((long double *)reduce(l1, (void *)(void *)sum));
	Test[index] = tot == *r;
	printf("Test %2d:\t%d/1\n", ++index, Test[index]);


	/*Insert with multithreding*/

	list *l3 = createList(TYPE_LONGDOUBLE);
	pthread_t threads[NUMBER_THREAD];

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		struct argThreads *t = malloc(sizeof(struct argThreads));
		long double *val = malloc(sizeof(long double));
		*val = (long double)i;
		t->value = val;
		t->l = l3;
		t->functionID = INSERT_THREADS;
		t->index = -1;
		t->function = NULL;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, (void *)t);
	}

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		pthread_join(threads[i], NULL);
	}

	Test[index] = numElements(l1, l3);
	if (Test[index] == -1)
		Test[index] = 0;
	printf("Test %d:\t%d/1\n", ++index, Test[index]);

	/*multithreading GetAt*/
	struct argThreads t[NUMBER_THREAD];

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		long double value = 0;
		t[i].value = &value;
		t[i].l = l1;
		t[i].functionID = GETAT_THREADS;
		t[i].index = i;
		t[i].ret = 1;
		t[i].function = NULL;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, (void *)t);
	}

	Test[index] = 1;

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		pthread_join(threads[i], NULL);
		if (t[i].ret == 0)
		{
			Test[index] = 0;
			break;
		}
	}

	printf("Test %d:\t%d/1\n", ++index, Test[index]);

	/*Reduce in multithreading*/

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		long double value = tot;
		t[i].value = &value;
		t[i].l = l1;
		t[i].functionID = REDUCE_THREADS;
		t[i].index = -1;
		t[i].ret = 0;
		t[i].function = (void*)(void *) sum;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, (void *)t);
	}

	Test[index] = 1;

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		pthread_join(threads[i], NULL);
		if (t[i].ret = 0)
		{
			Test[index] = 0;
			break;
		}
	}

	printf("Test %d:\t%d/1\n", ++index, Test[index]);

	/*

		printf("Testing!\n");
		long double a = 1, b = 2, c = 3, d = 4;
		list *testList = createList(TYPE_LONGDOUBLE), *secondTestList;
		insert(testList, &(a));
		insert(testList, &(b));
		insert(testList, &(c));
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

		//printList(secondTestList = map(testList, multiplyByThreeHundred));

		deleteList(secondTestList);

		printf("Reduced : %Lf\n", *((long double *)reduce(testList, sum)));
		printf("Reduced : %Lf\n", *((long double *)reduce(testList, min)));
		deleteList(testList);

		secondTestList = createList(TYPE_CHAR);
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

		printList(testList = map(secondTestList, shiftChars));

		deleteList(testList);

		deleteList(secondTestList);
	*/

	clock_t end = clock(); // End timer

	int result = 0;
	for (int i = 0; i < NUMBER_OF_TESTS; i++)
	{
		result += Test[i];
	}

	printf("Total passed tests:\t%d/%d\n", result, index);
	printf("Time:\t%.3lfms\n", (double)(end - begin) / CLOCKS_PER_SEC * 1000);
}