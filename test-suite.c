#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
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

int insertLongDouble (list * l, int lowerBound, int maxBound, int minValue){
	long double finalValue = 0;
	int value = minValue;
	for (int i = lowerBound; i<maxBound; i++){
		finalValue = (long double)value;
		insert(l, &(finalValue));
		if(l == NULL && errno == EINVAL) return -1;
		value++;
	}
	return 1;
}

int getAtLongDouble (list * l, int lowerBound, int maxBound, int minValue){
	long double finalValue = 0;
	int value = minValue;
	for (int i = lowerBound; i<maxBound; i++){
		finalValue = (long double)value;
		baseNode * n = getAt(l, maxBound-1 - i);
		if(n == NULL && errno == EINVAL) return -1;
		if(((ldoubleNode*)n)->value != finalValue){
			return 0;
		}
		
		value++;
	}
	return 1;
}

int main()
{
	int Test[50];
	for (int i = 0; i<50; i++){
		Test[i]= 1;
	}
	int index = 0;
	
	long double gh = 0;

	/*Test 1: creation of a List with invalid type*/
	list * l1 = createList(10);
	if(errno != EINVAL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);

	/*Test 2: ivalid remove from list*/
	baseNode * n = removeFromList(l1);
	if(errno != EINVAL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);


	/*Test 3: creation, insert and GetAt of a series of number in TYPE_LONGDOUBLE*/
	l1 = createList(TYPE_LONGDOUBLE);
	Test[index]= insertLongDouble(l1, 0, 10, 0);
	if(Test[index] == -1) printf("Ciao Test %d: %d/1\n", ++index, Test[index]);
	else{
		Test[index] = getAtLongDouble(l1, 0, 10, 0);
		printf("Test %d: %d/1\n", ++index, Test[index]);
	}
	printList(l1);

	/*Test 4: error GetAt in a list*/
	n = getAt(l1, 10000);
	if(errno != EINVAL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);


	/*Test 5: Error in the insert At*/
	gh = (long double)1;
	n = insertAt(l1, 10000, &gh);
	if(errno != EINVAL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);

	/*Test 6: insertAt of element*/
	gh = (long double)10;
	n = insertAt(l1, 0, &gh);
	Test[index] = getAtLongDouble(l1, 0, 11, 0);
	if(Test[index]==-1)Test[index]=0;
	printf("Test %d: %d/1\n", ++index, Test[index]);
	printList(l1);

	/*Test 7: removeAt of element*/
	n = removeFromListAt(l1, 0);
	Test[index] = getAtLongDouble(l1, 0, 10, 0);
	if(Test[index]==-1)Test[index]=0;
	printf("Test %d: %d/1\n", ++index, Test[index]);
	printList(l1);

	/*Test 8: error Remove At of an element*/
	n = removeFromListAt(l1, 10000);
	if(errno == EINVAL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);

	

/*

	TEST 5: remove of the list of new values in the long double list

	for (int i = 0; i<10; i++){
		gh = (long double)(i+10);
		baseNode *n = insertAt(l1, 0, &gh);
		//printf("VALUE: %Lf GH: %d\n", ((ldoubleNode*)n)->value, i);
		//printList(l1);
	}

	for (int i = 0; i<20; i++){
		gh = (long double)i;
		baseNode * n = getAt(l1, 19 - i);
		//printf("VALUE: %Lf i: %Lf\n", ((ldoubleNode*)n)->value, gh);
		if(((ldoubleNode*)n)->value != gh){
			Test[index]=0;
			break;
		}
	}

	printf("Test %d: %d/1\n", index, Test[index]);
	index++;

	TEST 6: removeFromList
	for (int i = 10; i<20; i++){
		gh = (long double)i;
		insert(l1, &(gh));
	}

	for (int i = 0; i<10; i++){
		removeFromList(l1);
	}

	for (int i = 0; i<10; i++){
		gh = (long double)i;
		baseNode * n = getAt(l1, 9 - i);
		//printf("VALUE: %Lf GH: %Lf\n", ((ldoubleNode*)n)->value, gh);
		if(((ldoubleNode*)n)->value != gh){
			Test[index]=0;
			break;
		}
	}

	printf("Test %d: %d/1\n",index, Test[index]);
	index++;



	printf("Testing!\n");
	long double a = 1, b = 2, c = 3, d = 4;
	list *testList = createList(TYPE_LONGDOUBLE), *secondTestList;
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

	printList( secondTestList = map(testList, multiplyByThreeHundred));

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
	int result = 0;
	for (int i = 0; i<50; i++){
		result += Test[i];
	}

	printf ("Total pass test: %d/50\n", result);
}