#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread_pool.h"

static void handleError(char *error)
{
	perror(error);
	exit(EXIT_FAILURE);
}

void *threadFunction(void *arg)
{
	threadPool *pool = (threadPool *)arg;
	while (1)
	{
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

			threadPoolJob *currentJob = malloc(sizeof(threadPoolJob)), *freeableJob = pool->firstJob;
			memcpy(currentJob, pool->firstJob, sizeof(threadPoolJob));

			if (pool->firstJob->next == NULL)
				// Handle case where there's only one job
				pool->lastJob = NULL;

			pool->firstJob = pool->firstJob->next;
			free (freeableJob);

			ret = sem_post(&(pool->accessingJobs));
			if (ret)
				handleError("sem_wait");

			jobResults *result = malloc(sizeof(jobResults));

			result->result = (currentJob->function)(currentJob->arg);
			result->resultID = currentJob->jobID;

			free(currentJob);

			ret = sem_wait(&(pool->accessingResults));
			if (ret)
				handleError("sem_wait");

			result->next = pool->firstResult;
			pool->firstResult = result;

			ret = sem_post(&(pool->numberOfResults));
			if (ret)
				handleError("sem_wait");

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
	pool->lastGeneratedJob = 0;

	int ret = sem_init(&(pool->numberOfJobs), 0, 0);
	if (ret)
		handleError("sem_init");

	ret = sem_init(&(pool->accessingJobs), 0, 1);
	if (ret)
		handleError("sem_init");
	ret = sem_init(&(pool->numberOfResults), 0, 0);
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

void deleteThreadPool(threadPool *pool)
{
	int ret = sem_wait(&(pool->accessingJobs));
	if (ret)
		handleError("sem_wait");
	ret = sem_wait(&(pool->accessingResults));
	if (ret)
		handleError("sem_wait");

	threadPoolJob *tempJob;
	while (pool->firstJob != NULL)
	{
		tempJob = pool->firstJob;
		pool->firstJob = tempJob->next;
		free(tempJob);
	}

	jobResults *tempResults;

	while (pool->firstResult != NULL)
	{
		tempResults = pool->firstResult;
		pool->firstResult = tempResults->next;
		free(tempResults);
	}

	for (int i = 0; i < THREAD_POOL_SIZE; i++)
	{
		pthread_cancel(pool->threads[i]);
		pthread_join(pool->threads[i], NULL);
	}
	ret = sem_destroy(&(pool->numberOfJobs));
	if (ret)
		handleError("sem_destroy");
	ret = sem_destroy(&(pool->accessingJobs));
	if (ret)
		handleError("sem_destroy");
	ret = sem_destroy(&(pool->numberOfResults));
	if (ret)
		handleError("sem_destroy");
	ret = sem_destroy(&(pool->accessingResults));
	if (ret)
		handleError("sem_destroy");

	free(pool);
}

unsigned int addJob(threadPool *pool, void *(*function)(void *), void *args)
{
	threadPoolJob *newJob = malloc(sizeof(threadPoolJob));

	newJob->arg = args;
	newJob->function = function;
	newJob->next = NULL;
	newJob->jobID = pool->lastGeneratedJob;
	pool->lastGeneratedJob++;

	int ret = sem_wait(&(pool->accessingJobs));
	if (ret)
		handleError("sem_wait");

	if (pool->lastJob == NULL)
	{
		// Handle case where there are no Jobs
		pool->firstJob = newJob;
	}
	else
	{
		pool->lastJob->next = newJob;
	}

	int newID = newJob->jobID;
	pool->lastJob = newJob;

	ret = sem_post(&(pool->numberOfJobs));
	if (ret)
		handleError("sem_wait");

	ret = sem_post(&(pool->accessingJobs));
	if (ret)
		handleError("sem_wait");

	return newID;
}

void *getResult(threadPool *pool, unsigned int resultID)
{
	void *result = NULL;
	int resultHasBeenFound = 0;

	while (!resultHasBeenFound)
	{
		int ret = sem_wait(&(pool->numberOfResults));
		if (ret)
			handleError("sem_wait");
		ret = sem_wait(&(pool->accessingResults));
		if (ret)
			handleError("sem_wait");

		jobResults *jobResult = pool->firstResult;

		if (jobResult != NULL)
		{
			if (jobResult->resultID == resultID)
			{
				pool->firstResult = pool->firstResult->next;
				result = jobResult->result;
				free(jobResult);
				resultHasBeenFound = 1;
			}
			else
			{
				while (jobResult != NULL && jobResult->next != NULL)
				{
					if (jobResult->next->resultID == resultID)
					{
						jobResults *foundResult = jobResult->next;
						result = foundResult->result;

						jobResult->next = foundResult->next;
						free(foundResult);
						resultHasBeenFound = 1;
						break;
					}
					jobResult = jobResult->next;
				}
			}
		}
		if (!resultHasBeenFound)
		{
			ret = sem_post(&(pool->numberOfResults));
			if (ret)
				handleError("sem_wait");
		}

		ret = sem_post(&(pool->accessingResults));
		if (ret)
			handleError("sem_wait");
	}
	return result;
}