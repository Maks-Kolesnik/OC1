#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "RWTest.h"

typedef struct {
	int threadNum;
	ThreadArray* arr;
} ThreadArgs;

typedef struct {
	pthread_t * thread;
	ThreadArgs* arg;
} Threads;

void threadArraySet(ThreadArray* arr, int index, double value) {
	pthread_rwlock_wrlock(&(arr->lock));
	arr->res[index] = value;
	pthread_rwlock_unlock(&(arr->lock));
}

double threadArrayGet(ThreadArray* arr, int index) {
	double res;
	pthread_rwlock_rdlock(&(arr->lock));
	res = arr->res[index];
	pthread_rwlock_unlock(&(arr->lock));
	return res;
}

void threadArrayToString(char* message, ThreadArray* arr) {
	pthread_rwlock_rdlock(&(arr->lock));
	printf("%s", message);
	for (int i = 0; i < arr->size; i++) {
		printf("%9.4f", arr->res[i]);
	}
	printf("\n");
	pthread_rwlock_unlock(&(arr->lock));
}

int getRandInt(int min, int max) {
	return rand() % (max - min) + min;
}

double getRandDouble(double min, double max) {
	return (double)rand() / RAND_MAX * (max - min) + min;
}

void* writerThreadFunc(void* arg) {
	srand(time(NULL));
	sleep(1);
	ThreadArgs* tArg = (ThreadArgs*)arg;
	
	int countOfReps = getRandInt(1, 10);

	printf("\n\tWriter Thread %d is started\n", tArg->threadNum);
	printf("\tWriter Thread %d make %d operations\n", tArg->threadNum, countOfReps);
	
	for (int i = 0; i < countOfReps; i++) {
		sleep(countOfReps % 4);
		int index = getRandInt(0, tArg->arr->size);
		double value = getRandDouble(0, tArg->threadNum);
		threadArraySet(tArg->arr, index, value);
		printf("\n\tWriter: %d\tRept: %d\t\tArr Index: %d\tValue: %lf\n", tArg->threadNum, i+1, index, value);
	}

	printf("\n\tWriter %d is stopped\n", tArg->threadNum);
	return NULL;
}

void* readerThreadFunc(void* arg) {
	srand(time(NULL));
	sleep(1);
	ThreadArgs* tArg = (ThreadArgs*)arg;
	
	int countOfReps = getRandInt(1, 10);

	printf("\n\tReader Thread %d is started\n", tArg->threadNum);
	printf("\tReader Thread %d make %d operations\n", tArg->threadNum, countOfReps);

	for (int i = 0; i < countOfReps; i++) {
		sleep(countOfReps % 4);
		int index = getRandInt(0, tArg->arr->size);
		double res = threadArrayGet(tArg->arr, index);
		printf("\n\tReader: %d\tRept: %d\t\tArr Index: %d\tValue: %lf\n", tArg->threadNum, i+1, index, res);
	}

	printf("\n\tReader %d is stopped\n", tArg->threadNum);
	return NULL;
}

void* detachThreadFunc(void* arg) {
	ThreadArray arr = *(ThreadArray*)arg;

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) {
		printf("Error while changing cancel state\n");
		exit(1);
	}

	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)) {
		printf("Error while changing cancel type\n");
		exit(1);
	}
	
	sleep(1);
	printf("\tStarting detached thread\n");

	while (1) {
		sleep(1);
		threadArrayToString("\nActual array: ", &arr);
	}

	printf("\tFinishing detached thread\n");
	return NULL;
}

pthread_t createDetachedThread(ThreadArray* arr) {
	pthread_attr_t attr;
	pthread_t thread;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (pthread_create(&thread, &attr, &detachThreadFunc, (void*)arr)) {
		printf("Error while creating detached thread\n");
		exit(1);
	}
	pthread_attr_destroy(&attr);

	return thread;
}

Threads* createWriterThreads(ThreadArgs* targ) {
	Threads* writerThreads;
	ThreadArgs* arg;

	writerThreads = (Threads*)malloc(sizeof(Threads));
	if (!writerThreads) {
		printf("Error while allocating memory\n");
		exit(1);
	}

	writerThreads->thread = (pthread_t*)malloc(targ->threadNum * sizeof(pthread_t));
	if (!writerThreads->thread) {
		printf("Error while allocating memory\n");
		exit(1);
	}

	writerThreads->arg = (ThreadArgs*)malloc(targ->threadNum * sizeof(ThreadArgs));
	if (!writerThreads->arg) {
		printf("Error while allocating memory\n");
		exit(1);
	}

	for (int i = 0; i < targ->threadNum; i++) {
		writerThreads->arg[i].threadNum = i;
		writerThreads->arg[i].arr = targ->arr;
		if (pthread_create(&(writerThreads->thread[i]), NULL, &writerThreadFunc, &(writerThreads->arg[i]))) {
			printf("Error while creating writer thread\n");
			exit(1);
		}
	}

	return writerThreads;
}

Threads* createReaderThreads(ThreadArgs* targ) {
	Threads* readerThreads;
	ThreadArgs* arg;

	readerThreads = (Threads*)malloc(sizeof(Threads));
	if (!readerThreads) {
		printf("Error while allocating memory\n");
		exit(1);
	}

	readerThreads->thread = (pthread_t*)calloc(targ->threadNum, sizeof(pthread_t));
	if (!readerThreads->thread) {
		printf("Error while allocating memory\n");
		exit(1);
	}

	readerThreads->arg = (ThreadArgs*)calloc(targ->threadNum, sizeof(ThreadArgs));
	if (!readerThreads->arg) {
		printf("Error while allocating memory\n");
		exit(1);
	}

	for (int i = 0; i < targ->threadNum; i++) {
		
		readerThreads->arg[i].threadNum = i;
		readerThreads->arg[i].arr = targ->arr;
		if (pthread_create(&(readerThreads->thread[i]), NULL, &readerThreadFunc, &(readerThreads->arg[i]))) {
			printf("Error while creating reader thread\n");
			exit(1);
		}
	}

	return readerThreads;
}

void RWTesting(ProgConf* init) {
	pthread_t detachedThread;
	ThreadArgs writerArgs, readerArgs;
	Threads* writers, *readers;
	
	printf("Count of writer threads: %d\n", init->writersCount);
	printf("Count of readers threads: %d\n", init->readersCount);
	threadArrayToString("Initial array: ", init->arr);

	detachedThread = createDetachedThread(init->arr);

	writerArgs.arr = init->arr;
	writerArgs.threadNum = init->writersCount;
	writers = createWriterThreads(&writerArgs);

	readerArgs.arr = init->arr;
	readerArgs.threadNum = init->readersCount;
	readers = createReaderThreads(&readerArgs);
	
	for (int i = 0; i < init->writersCount; i++) {
		if (pthread_join(writers->thread[i], NULL) != 0) {
			printf("Error while joining writer thread\n");
			exit(1);
		}
	}

	for (int i = 0; i < init->readersCount; i++) {
		if (pthread_join(readers->thread[i], NULL) != 0) {
			printf("Error while joining reader thread\n");
			exit(1);
		}
	}
	
	{
		free(writers->thread);
		free(writers->arg);
		free(writers);

		free(readers->thread);
		free(readers->arg);
		free(readers);
	}
	
	if(pthread_cancel(detachedThread) != 0) {
		printf("Error while cancelling detached thread\n");
		exit(1);
	}
	
	threadArrayToString("Array after operations: ", init->arr);
	return;
}
