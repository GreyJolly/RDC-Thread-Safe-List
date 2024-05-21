#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "thread-safe-list.h"

#define NUMBER_OF_TESTS 50
#define NUMBER_THREAD 10
#define NUMBER_ELEMENTS 10

typedef struct argThreads
{
	list *l;
	list *compare_list;
	void *value;
	int functionID;
	int index;
	int ret;
	void *(*function)(void *, void *);

} argThreads;

enum ThreadsFunctions
{
	INSERT_THREADS,
	GETAT_THREADS,
	REMOVEFROMLIST_THREADS,
	MAP_THREADS,
	REDUCE_THREADS
};

long double *sum(long double *n1, long double *n2)
{
	long double *returnValue = malloc(sizeof(long double));
	*returnValue = *n1 + *n2;

	return returnValue;
}

long double *min(long double *n1, long double *n2)
{
	return *n1 < *n2 ? n1 : n2;
}

long double *multiplyByTwo(long double *number)
{
	long double *returnValue = malloc(sizeof(long double));
	*returnValue = (*number) * 2;

	return returnValue;
}

char *shiftChars(char *chars)
{
	char *returnValue;

	for (int i = 0; i < 7; i++)
	{
		returnValue[i] = chars[i] + 1;
	}
	returnValue[7] = '\0';

	return returnValue;
}

char *sortAlphabetically(char *chars1, char *chars2)
{
	return (memcmp(chars1, chars2, TYPE_CHAR_LENGTH) < 0) ? chars1 : chars2;
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

// Counts the number of occurences of an element in a list
int countOccurences(list *l, baseNode *n)
{
	int occurences = 0;
	baseNode *iter;
	switch (l->listType)
	{
	case TYPE_CHAR:
		for (int i = 0; 1; i++)
		{
			iter = getAt(l, i);
			if (iter == NULL)
				break;
			if (memcmp(((charNode *)iter)->value, ((charNode *)n)->value, 8) == 0)
				occurences++;
		}
		break;
	case TYPE_LONGDOUBLE:
		for (int i = 0; 1; i++)
		{
			iter = getAt(l, i);
			if (iter == NULL)
				break;
			if (((ldoubleNode *)iter)->value == ((ldoubleNode *)n)->value);
				occurences++;
		}
		break;
	}
	return occurences;
}

// Checks whether two lists have the same number of elements
int sameNumberOfElements(list *l1, list *l2)
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

// Checks whehter two lists have the same elements, regardless of order
int checkSameElements(list *l1, list *l2)
{
	if (l1->listType != l2->listType || sameNumberOfElements(l1,l2) != 1)
		return 0;
	baseNode *n;
	for (int i = 0; 1; i++)
	{
		n = getAt(l1, i);
		if (n == NULL)
			return 1;
		if (countOccurences(l1, n) != countOccurences(l2, n))
			return 0;
	}
	return 1;
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
		break;
	case GETAT_THREADS:
		n = getAt(argv->l, argv->index);
		if (errno == EINVAL && n == NULL)
		{
			argv->ret = 0;
		}
		if ((long double)(NUMBER_THREAD - 1 - argv->index) != ((ldoubleNode *)n)->value)
		{
			argv->ret = 0;
		}
		break;

	case REDUCE_THREADS:
		long double *r = ((long double *)reduce(argv->l, (void *)(void *)argv->function));
		if (errno == EINVAL && r == NULL)
		{
			argv->ret = 0;
		}
		else
			argv->ret = *(long double *)(argv->value) == *r;
		break;

	case MAP_THREADS:
		list *res = map(argv->l, (void *)(void *)argv->function);
		if (errno == EINVAL && res == NULL)
		{
			argv->ret = 0;
		}
		else
			argv->ret = checkSameElements(res, argv->compare_list);
		break;

	case REMOVEFROMLIST_THREADS:
		n = removeFromList(argv->l);
		break;
	}
}

int main()
{
	printf("Started testing...\n\n");
	int Test[NUMBER_OF_TESTS];
	for (int i = 0; i < NUMBER_OF_TESTS; i++)
	{
		Test[i] = 0;
	}
	int index = 0;

	long double test_longDouble = 0;
	char test_char[8];

	// Start timing tests
	clock_t begin = clock();

	printf("Testing lists with char\n");

	/*TEST 1: Creation of a List with invalid type*/
	list *l = createList(NUMBER_ELEMENTS);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tCreation of a List with invalid type\n", ++index, Test[index]);

	/*TEST 2: Invalid remove from list*/
	baseNode *n = removeFromList(l);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tInvalid remove from list\n", ++index, Test[index]);

	/*TEST 3: Creation, insert and GetAt of a series of number in TYPE_CHAR*/
	list *list_char = createList(TYPE_CHAR);
	Test[index] = insertChar(list_char, 0, NUMBER_ELEMENTS, 'a');
	if (Test[index] != -1)
		Test[index] = getAtChar(list_char, 0, NUMBER_ELEMENTS, 'a');

	printf("\tTest %2d:\t%d/1\t\tCreation, insert and GetAt of a series of number in TYPE_CHAR\n", ++index, Test[index]);

	/*TEST 4: Error GetAt in a list*/
	n = getAt(list_char, 10000);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tError GetAt in a list\n", ++index, Test[index]);

	/*TEST 5: Error in the insert At*/
	memcpy(test_char, "ciaone1a", TYPE_CHAR_LENGTH);
	n = insertAt(list_char, 10000, &test_char);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tError in the insertAt\n", ++index, Test[index]);

	/*TEST 6: insertAt of element*/
	Test[index] = 1;
	memcpy(test_char, "ciaone1a", TYPE_CHAR_LENGTH);
	n = insertAt(list_char, 0, &test_char);
	for(int i = 0; i<NUMBER_ELEMENTS+1; i++){
		n = getAt(list_char, i);
		if(n==NULL) Test[index] = 0;
	}
	printf("\tTest %2d:\t%d/1\t\tinsertAt of element\n", ++index, Test[index]);

	/*TEST 7: removeAt of element*/
	Test[index] = 1;
	n = removeFromListAt(list_char, 0);
	for(int i = 0; i<NUMBER_ELEMENTS; i++){
		n = getAt(list_char, i);
		if(n==NULL) Test[index] = 0;
	}
	printf("\tTest %2d:\t%d/1\t\tremoveAt of element\n", ++index, Test[index]);

	/*TEST 8: Error Remove At of an element */
	n = removeFromListAt(list_char, 10000);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tError removeAt of an element\n", ++index, Test[index]);

	/*TEST 9: Remove of an element*/
	Test[index] = 1;
	n = insertAt(list_char, 0, &test_char);
	if (errno == EINVAL && n == NULL)
	{
		Test[index] = 0;
	}
	if(Test[index]!=0){
		n = removeFromList(list_char);
		if (errno == EINVAL && n == NULL)
		{
			Test[index] = 0;
		}
		for(int i = 0; i<NUMBER_ELEMENTS; i++){
			n = getAt(list_char, i);
			if(n==NULL) Test[index] = 0;
		}
	}
	printf("\tTest %2d:\t%d/1\t\tRemove of an element\n", ++index, Test[index]);

	/* TEST 10: Error in map, list inavlid*/
	list *list_char_2 = map(NULL, (void *)(void *)shiftChars);
	Test[index] = errno == EINVAL && list_char_2 == NULL;
	printf("\tTest %2d:\t%d/1\t\tError in map, list inavlid\n", ++index, Test[index]);
	
	/*TEST 11: Error in map, function invalid */
	list_char_2 = map(list_char, NULL);
	Test[index] = errno == EINVAL && list_char_2 == NULL;
	printf("\tTest %2d:\t%d/1\t\tError in map, function invalid\n", ++index, Test[index]);


	/*Creation of a List with invalid type*/
	list *l1 = createList(NUMBER_ELEMENTS);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tCreation of a List with invalid type\n", ++index, Test[index]);

	/*Invalid remove from list*/
	n = removeFromList(l1);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tInvalid remove from list\n", ++index, Test[index]);

	/*Creation, insert and GetAt of a series of number in TYPE_LONGDOUBLE*/
	l1 = createList(TYPE_LONGDOUBLE);
	Test[index] = insertLongDouble(l1, 0, NUMBER_ELEMENTS, 0);
	if (Test[index] != -1)
		Test[index] = getAtLongDouble(l1, 0, NUMBER_ELEMENTS, 0);
	printf("\tTest %2d:\t%d/1\t\tCreation, insert and GetAt of a series of number in TYPE_LONGDOUBLE\n", ++index, Test[index]);

	/*Error GetAt in a list*/
	n = getAt(l1, 10000);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tError GetAt in a list\n", ++index, Test[index]);

	/*Error in the insert At*/
	test_longDouble = (long double)1;
	n = insertAt(l1, 10000, &test_longDouble);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tError in the insertAt\n", ++index, Test[index]);

	/*insertAt of element*/
	test_longDouble = (long double)10;
	n = insertAt(l1, 0, &test_longDouble);
	Test[index] = getAtLongDouble(l1, 0, 11, 0);
	if (Test[index] == -1)
		Test[index] = 0;
	printf("\tTest %2d:\t%d/1\t\tinsertAt of element\n", ++index, Test[index]);

	/*removeAt of element*/
	n = removeFromListAt(l1, 0);
	Test[index] = getAtLongDouble(l1, 0, 10, 0);
	if (Test[index] == -1)
		Test[index] = 0;
	printf("\tTest %2d:\t%d/1\t\tremoveAt of element\n", ++index, Test[index]);

	/*Error Remove At of an element*/
	n = removeFromListAt(l1, 10000);
	Test[index] = errno == EINVAL;
	printf("\tTest %2d:\t%d/1\t\tError removeAt of an element\n", ++index, Test[index]);

	/*Remove of an element*/
	list *base_list = createList(TYPE_LONGDOUBLE);
	Test[index] = insertLongDouble(base_list, 0, NUMBER_ELEMENTS, 0);
	if (Test[index] != -1)
		Test[index] = getAtLongDouble(l1, 0, NUMBER_ELEMENTS, 0);

	test_longDouble = (double long)10;

	n = insert(l1, &test_longDouble);
	if (errno == EINVAL && n == NULL)
	{
		Test[index] = 0;
	}

	n = removeFromList(l1);

	if (errno == EINVAL && n == NULL)
	{
		Test[index] = 0;
		printf("SONO QUA\n");
	}
	else
	{
		Test[index] = checkSameElements(base_list, l1);
	}
	printf("\tTest %2d:\t%d/1\t\tRemove of an element\n", ++index, Test[index]);

	/*Error in map, list inavlid*/
	list *l2 = map(NULL, (void *)(void *)multiplyByTwo);
	Test[index] = errno == EINVAL && l2 == NULL;
	printf("\tTest %2d:\t%d/1\t\tError in map, list inavlid\n", ++index, Test[index]);

	/*Error in map, function invalid */
	l2 = map(l1, NULL);
	Test[index] = errno == EINVAL && l2 == NULL;
	printf("\tTest %2d:\t%d/1\t\tError in map, function invalid\n", ++index, Test[index]);

	/*Map*/
	list *DoubleList = createList(TYPE_LONGDOUBLE);
	for (int i = 0; i < 10; i++)
	{
		test_longDouble = (long double)(i * 2);
		baseNode *node = insert(DoubleList, &test_longDouble);
	}

	l2 = map(l1, (void *)(void *)multiplyByTwo);
	if (l2 == NULL && errno == EINVAL)
		Test[index] = 0;
	Test[index] = checkSameElements(l2, DoubleList);
	printf("\tTest %d:\t%d/1\t\tMap in single thread\n", ++index, Test[index]);

	/*Error in reduce: list invalid*/
	long double *r = ((long double *)reduce(NULL, (void *)(void *)sum));
	Test[index] = errno == EINVAL && r == NULL;
	printf("\tTest %2d:\t%d/1\t\tError in reduce: list invalid\n", ++index, Test[index]);

	/*Error in reduce: function invalid*/
	r = ((long double *)reduce(l1, NULL));
	Test[index] = errno == EINVAL && r == NULL;
	printf("\tTest %2d:\t%d/1\t\tError in reduce: function invalid\n", ++index, Test[index]);

	/*reduce*/
	long double tot = 0;
	for (int i = 0; i < NUMBER_ELEMENTS; i++)
	{
		baseNode *n = getAt(l1, i);
		tot += ((ldoubleNode *)n)->value;
	}
	r = ((long double *)reduce(l1, (void *)(void *)sum));
	Test[index] = tot == *r;
	printf("\tTest %2d:\t%d/1\t\tReduce in single thread\n", ++index, Test[index]);

	/*Insert with multithreding*/
	list *l3 = createList(TYPE_LONGDOUBLE);
	pthread_t threads[NUMBER_THREAD];
	struct argThreads t[NUMBER_THREAD];

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		t[i].value = malloc(sizeof(long double));
		*(long double *)(t[i].value) = (long double)i;
		t[i].l = l3;
		t[i].functionID = INSERT_THREADS;
		t[i].index = -1;
		t[i].function = NULL;
		t[i].compare_list = NULL;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, &t[i]);
	}

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		pthread_join(threads[i], NULL);
		free(t[i].value);
	}

	Test[index] = checkSameElements(l1, l3);

	Test[index] = sameNumberOfElements(l1, l3);
	if (Test[index] == -1)
		Test[index] = 0;
	printf("\tTest %d:\t%d/1\t\tInsert with multithreding\n", ++index, Test[index]);

	/*Multithreading RemoveAt*/
	Test[index] = insertLongDouble(l1, NUMBER_ELEMENTS, NUMBER_ELEMENTS + NUMBER_THREAD, NUMBER_ELEMENTS);

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		long double value = 0;
		t[i].value = &value;
		t[i].l = l1;
		t[i].functionID = REMOVEFROMLIST_THREADS;
		t[i].index = i;
		t[i].ret = 1;
		t[i].function = NULL;
		t[i].compare_list = NULL;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, &t[i]);
	}

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		pthread_join(threads[i], NULL);
	}

	Test[index] = sameNumberOfElements(l1, l3);
	if (Test[index] == -1)
		Test[index] = 0;
	printf("\tTest %d:\t%d/1\t\tRemoveAt with multithreding\n", ++index, Test[index]);

	/*multithreading GetAt*/

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		long double value = 0;
		t[i].value = &value;
		t[i].l = l1;
		t[i].functionID = GETAT_THREADS;
		t[i].index = i;
		t[i].ret = 1;
		t[i].function = NULL;
		t[i].compare_list = NULL;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, &t[i]);
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

	printf("\tTest %d:\t%d/1\t\tTesting GetAt in multithreading\n", ++index, Test[index]);

	/*map in multithreading*/

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		long double value = tot;
		t[i].value = &value;
		t[i].l = l1;
		t[i].functionID = MAP_THREADS;
		t[i].index = -1;
		t[i].ret = 0;
		t[i].compare_list = DoubleList;
		t[i].function = (void *)(void *)multiplyByTwo;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, &t[i]);
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

	deleteList(DoubleList);

	printf("\tTest %d:\t%d/1\t\tTesting map in multithreading \n", ++index, Test[index]);

	/*Reduce in multithreading*/

	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		long double value = tot;
		t[i].value = &value;
		t[i].l = l1;
		t[i].compare_list = NULL;
		t[i].functionID = REDUCE_THREADS;
		t[i].index = -1;
		t[i].ret = 0;
		t[i].function = (void *)(void *)sum;
		pthread_create(threads + i, NULL, (void *)testThreadFunction, &t[i]);
		pthread_detach(threads[i]);
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

	printf("\tTest %d:\t%d/1\t\tTesting reduce in multithreading\n", ++index, Test[index]);

	deleteList(list_char);		// Move if necessary
	deleteList(list_char_2);	// Doing some leak testing
	deleteList(l1);
	deleteList(l2);
	deleteList(l3);
	deleteList(base_list);

	clock_t end = clock(); // End timer

	int result = 0;
	for (int i = 0; i < NUMBER_OF_TESTS; i++)
	{
		result += Test[i];
	}

	printf("\nTotal passed tests:\t%d/%d\n", result, index);
	printf("\nTime:\t%.3lfms\n", (double)(end - begin) / CLOCKS_PER_SEC * 1000);
}