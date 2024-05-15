#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // TODO: REMOVE
#include "thread_pool.h"

void printJob(threadPoolJob *job)
{
	if (job == NULL)
	{
		printf("Job is null!");
		fflush(stdout);
	}
	else
	{
		printf("Printing job: {function: %d  arg: %Lf, jobID: %d, next: %d}\n", job->function, *((long double *)job->arg), job->jobID, job->next);
		fflush(stdout);
	}
}

void printResult(jobResults *results)
{
	if (results == NULL)
	{
		printf("results is null0!\n");
		fflush(stdout);
	}
	else
	{
		printf("Printing results: {result: %Lf, jobID: %d, next: %d}\n", *((long double *)results->result), results->jobID, results->next);
		fflush(stdout);
	}
}

static void handleError(char *error)
{
	perror(error);
	exit(EXIT_FAILURE);
}

void *threadFunction(void *arg)
{
	printf("Thread started...\n");
	threadPool *pool = (threadPool *)arg;
	while (1)
	{
		sleep(2); // TODO: REMOVE
		// Wait for an available job
		int ret = sem_wait(&(pool->numberOfJobs));
		if (ret)
			handleError("sem_wait");

		ret = sem_wait(&(pool->accessingJobs));
		if (ret)
			handleError("sem_wait");

		// If present, get a job from the queue
		if (pool->firstJob != NULL)
		{

			threadPoolJob *currentJob = malloc(sizeof(threadPoolJob));
			memcpy(currentJob, pool->firstJob, sizeof(threadPoolJob));

			if (pool->firstJob->next == NULL)
				// Handle case where there's only one job
				pool->lastJob = NULL;

			pool->firstJob = pool->firstJob->next;

			for (threadPoolJob *i = pool->firstJob; i != NULL; i = i->next)
			{
				i->jobID--;
			}

			ret = sem_post(&(pool->accessingJobs));
			if (ret)
				handleError("sem_wait");

			jobResults *result = malloc(sizeof(jobResults));

			result->result = (currentJob->function)(currentJob->arg);
			result->jobID = currentJob->jobID;

			free(currentJob);

			ret = sem_wait(&(pool->accessingResults));
			if (ret)
				handleError("sem_wait");

			result->next = pool->firstResult;
			pool->firstResult = result;

			printf("Added result: ");
			printResult(pool->firstResult);

			ret = sem_post(&(pool->accessingResults));
			if (ret)
				handleError("sem_wait");
		}
		else
		{

			ret = sem_post(&(pool->accessingJobs));
			if (ret)
				handleError("sem_wait");
		}
	}
}

threadPool *createThreadPool()
{
	threadPool *pool = malloc(sizeof(threadPool));

	pool->firstJob = NULL;
	pool->lastJob = NULL;
	pool->firstResult = NULL;

	int ret = sem_init(&(pool->numberOfJobs), 0, 0);
	if (ret)
		handleError("sem_init");
	ret = sem_init(&(pool->accessingJobs), 0, 1);
	if (ret)
		handleError("sem_init");
	ret = sem_init(&(pool->accessingResults), 0, 1);
	if (ret)
		handleError("sem_init");

	for (int i = 0; i < THREAD_POOL_SIZE; i++)
	{
		ret = pthread_create(pool->threads + i, NULL, threadFunction, pool);
	}

	return pool;
}

void deleteThreadPool()
{
	// TODO: implement
}

void addJob(threadPool *pool, void *(*function)(void *), void *args)
{
	threadPoolJob *newJob = malloc(sizeof(threadPoolJob));

	newJob->arg = args;
	newJob->function = function;
	newJob->next = NULL;

	int ret = sem_wait(&(pool->accessingJobs));
	if (ret)
		handleError("sem_wait");

	if (pool->lastJob == NULL)
	{
		// Handle case where there are no Jobs
		newJob->jobID = 0;
		pool->firstJob = newJob;
	}
	else
	{
		newJob->jobID = pool->lastJob->jobID + 1;
		pool->lastJob->next = newJob;
	}

	pool->lastJob = newJob;

	printf("Added job: ");
	printJob(pool->lastJob);

	ret = sem_post(&(pool->numberOfJobs));
	if (ret)
		handleError("sem_wait");

	ret = sem_post(&(pool->accessingJobs));
	if (ret)
		handleError("sem_wait");
}

void *getFirstResult(threadPool *pool)
{
	void *result;

	int ret = sem_wait(&(pool->accessingResults));
	if (ret)
		handleError("sem_wait");

	jobResults *jobResult = pool->firstResult;

	if (jobResult->next = NULL)
	{
		// Handle case where there is only one element
		pool->firstResult = NULL;
		result = jobResult->result;
		free(jobResult);
	}
	while (jobResult->next != NULL)
	{
		if (jobResult->next->jobID == 0)
		{
			jobResults *firstResult = jobResult->next;
			result = firstResult->result;
			jobResult->next = firstResult->next;
			free(firstResult);
		}
	}

	for (jobResult = pool->firstResult; jobResult != NULL; jobResult = jobResult->next)
	{
		jobResult->jobID--;
	}

	ret = sem_post(&(pool->accessingResults));
	if (ret)
		handleError("sem_wait");

	return result;
}