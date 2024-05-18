#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "thread-safe-list.h"
#include <pthread.h>

typedef struct argThreads
{
	list *l;
	void *value;
	int functionID;
}argThreads;

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

void *sum(void *n1, void *n2)
{ 
	long double *test = malloc(sizeof(long double));
	;
	*test = (*((long double *)n1) + *((long double *)n2));

	return test;
}

void *min(void *n1, void *n2)
{
	long double *test = malloc(sizeof(long double));

	*test = (*((long double *)n1) < *((long double *)n2)) ? *((long double *)n1) : *((long double *)n2);
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

int insertLongDouble (list * l, int lowerBound, int upperBound, int minValue){
	long double finalValue = 0;
	int value = minValue;
	for (int i = lowerBound; i < upperBound; i++)
	{
		finalValue = (long double)value;
		baseNode * n = insert(l, &(finalValue));
		if(n == NULL && errno == EINVAL) return -1;
		value++;
	}
	return 1;
}

int insertChar (list * l,  int lowerBound, int upperBound, char startingChar) {
	char value[8] = {startingChar, startingChar+1, startingChar+2, startingChar+3, startingChar+4, startingChar+5, startingChar+6, startingChar+7};
	for (int i = lowerBound; i<upperBound; i++)  {
		for (int j = 0; j<8; j++) {
			value[j] = startingChar + j;
		}
		baseNode * n = insert(l, value);
		if(n == NULL && errno == EINVAL) return -1;
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
		baseNode * n = getAt(l, upperBound-1 - i);
		if(n == NULL && errno == EINVAL) return -1;
		if(((ldoubleNode*)n)->value != finalValue){
			return 0;
		}
		
		value++;
	}
	return 1;
}

int insertChar() {

}

int compareList(list * l1, list * l2){
	int index = 0;
	baseNode * n1 = getAt(l1, index);
	baseNode* n2 = getAt(l2, index);
	while (n1 != NULL && n2 != NULL){
		if(n1 == NULL && errno == EINVAL) return -1;
		if(n2 == NULL && errno == EINVAL) return -1;
		if(((ldoubleNode*)n1)->value != ((ldoubleNode*)n2)->value) return 0;
		index++;
		n1 = getAt(l1, index);
		n2 = getAt(l2, index);
	}

	return 1;
}

void threadFunction(struct argThreads* argv){
	switch(argv->functionID){
		case INSERT_THREADS:
		for (int i = 0; i<10; i++){
			
			baseNode * n = insert (argv->l, argv->value);
			argv->value++;
			/*TODO: exit control*/
			
		}
		break;
	}

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

	/*Test 2: invalid remove from list*/
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
	if(errno != EINVAL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);

	/* Test 9: Error in map, list inavlid*/
	list *l2 = map(NULL , multiplyByTwo);
	if(errno != EINVAL && l2 != NULL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);

	/*Test 10: Error in map, function invalid */
	l2 = map( l1, NULL);
	if(errno != EINVAL && l2 != NULL) Test[index] = 0;
	printf("Test %d: %d/1\n", ++index, Test[index]);

	/*Test 11: map*/
	list* DoubleList = createList(TYPE_LONGDOUBLE);
	for(int i = 0; i<10; i++){
		gh = (long double)(i*2);
		baseNode * node = insert(DoubleList, &gh);
	}
	printList(DoubleList);

	l2 = map( l1, multiplyByTwo);
	if(l2 == NULL && errno == EINVAL)Test[index]=0;
	Test[index] = compareList(l2, DoubleList);
	printList(l2);
	printf("Test %d: %d/1\n", ++index, Test[index]);

	/*Test 12: insert with multithreding*/

	list* l3 = createList(TYPE_LONGDOUBLE);
	pthread_t threads;
	struct argThreads* t = malloc(sizeof(struct argThreads));

	for (int i = 0; i < 10; i++)
	{
		long double * val = malloc(sizeof(long double));
		*val = i;
		t->value = val;
		t->l = l3;
		t->functionID = INSERT_THREADS;
		pthread_create(&threads, NULL, (void*)threadFunction, (void*)t);
	}

	for (int i = 0; i < 10; i++)
	{
		pthread_join(threads, NULL);
	}

	Test[index] = compareList(l3, l1);
	printf("Test %d: %d/1\n", ++index, Test[index]);

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
	int result = 0;
	for (int i = 0; i<50; i++){
		result += Test[i];
	}

	printf ("Total pass test: %d/50\n", result);
}