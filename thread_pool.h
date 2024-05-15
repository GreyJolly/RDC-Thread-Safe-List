#include <semaphore.h>
#include <pthread.h>

#define THREAD_POOL_SIZE 8
#define MAXIMUM_RESULT_SIZE 16

typedef struct threadPoolJob
{
	void *(*function)(void *);
	void *arg;
	struct threadPoolJob *next;
	unsigned int jobID;
} threadPoolJob;

typedef struct jobResults
{
	void *result;
	struct jobResults *next;
	unsigned int jobID;
} jobResults;

typedef struct threadPool
{
	threadPoolJob *firstJob;
	threadPoolJob *lastJob;
	jobResults *firstResult;
	sem_t numberOfJobs;
	sem_t accessingJobs;
	sem_t accessingResults;
	pthread_t threads[THREAD_POOL_SIZE];
} threadPool;

threadPool *createThreadPool();
void addJob(threadPool *pool, void *(*function)(void *), void *args);
void *getFirstResult(threadPool *pool);