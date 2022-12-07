#include <sys/ipc.h>

#include "lib.h"

SHMType* createSHM(int size) {
	pthread_mutexattr_t m_attr;
	SHMType* shm;

	int shm_id = shmget(IPC_PRIVATE, sizeof(SHMType), IPC_CREAT | IPC_EXCL | 0200 | 0400);
	if (shm_id == -1) {
		printf("\nError! Can`t create shared memory\n");
		exit(1);
	}

	shm = (SHMType*)shmat(shm_id, NULL, 0);

	if (shm == (SHMType*)-1) {
		printf("\nError! Can`t connect to shared memory\n");
		exit(1);
	}

	shm->shm_id = shm_id;
	shm->size = size;

	if ((shm->shmBufferID = shmget(IPC_PRIVATE, size * sizeof(int), IPC_CREAT | IPC_EXCL | 0200 | 0400)) == -1) {
		printf("\nError! Can`t create shared memory\n");
		exit(1);
	}

	
	pthread_mutexattr_init(&m_attr);
	pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
	pthread_mutexattr_setrobust(&m_attr, PTHREAD_MUTEX_ROBUST);

	if (pthread_mutex_init(&shm->lock, &m_attr) != 0) {
		printf("\nError! Can`t initialize mutex!\n");
		exit(1);
	}

	pthread_mutexattr_destroy(&m_attr);

	if (sem_init(&(shm->emptyBuffer), 1, 1) != 0) {
		printf("\nError! Can`t initialize empty buffer semaphore\n");
		exit(1);
	}

	if (sem_init(&(shm->notEmptyBuffer), 1, 0) != 0) {
		printf("\nError! Can`t initialize full buffer semaphore\n");
		exit(1);
	}

	if (sem_init(&(shm->isResultInBuffer), 1, 0) != 0) {
		printf("\nError! Can`t initialize result buffer semaphore\n");
		exit(1);
	}

	return shm;
}

void setBuffer(SHMType* shm, int* arr, int size) {
	int i;

	sem_wait(&(shm->emptyBuffer));
	pthread_mutex_lock(&(shm->lock));
	
	if ((shm->buffer = (int*) shmat(shm->shmBufferID, NULL, 0)) == (int*) -1) {
		printf("\nError! Can`t connect to shared memory\n");
		exit(EXIT_FAILURE);
	}
	
	shm->buffer[0] = size;
	for (i = 1; i <= size; i++) {
		shm->buffer[i] = arr[i - 1];
	}
	
	shmdt(shm->buffer);

	pthread_mutex_unlock(&(shm->lock));
	sem_post(&(shm->notEmptyBuffer));
}

void calculationSum(SHMType* shm) {
	double res = 0;
	int i;

	sem_wait(&(shm->notEmptyBuffer));
	pthread_mutex_lock(&(shm->lock));

	if ((shm->buffer = (int*) shmat(shm->shmBufferID, NULL, 0)) == (int*) -1) {
		printf("\nError! Can`t connect to shared memory\n");
		exit(EXIT_FAILURE);
	}

	for (i = 1; i <= shm->buffer[0]; i++) {
		res += shm->buffer[i];
		shm->buffer[i] = 0;
	}
	shm->buffer[0] = res;
	
	shmdt(shm->buffer);

	pthread_mutex_unlock(&(shm->lock));
	sem_post(&(shm->isResultInBuffer));
}

int getResult(SHMType* shm) {
	int res;

	sem_wait(&(shm->isResultInBuffer));
	pthread_mutex_lock(&(shm->lock));
	
	if ((shm->buffer = (int*) shmat(shm->shmBufferID, NULL, 0)) == (int*) -1) {
		printf("\nError! Can`t connect to shared memory\n");
		exit(EXIT_FAILURE);
	}

	res = shm->buffer[0];
	shm->buffer[0] = 0;

	shmdt(shm->buffer);
	
	pthread_mutex_unlock(&(shm->lock));
	sem_post(&(shm->emptyBuffer));

	return res;
}

void deleteSHM(SHMType* shm) {
	int shm_id, res1, res2, res3;

	shmdt(shm->buffer);
	shmctl(shm->shmBufferID, IPC_RMID, NULL);

	shm->buffer = NULL;
	shm->size = 0;

	pthread_mutex_destroy(&(shm->lock));
	sem_destroy(&(shm->emptyBuffer));
	sem_destroy(&(shm->notEmptyBuffer));
	sem_destroy(&(shm->isResultInBuffer));

	shm_id = shm->shm_id;
	shmctl(shm_id, IPC_RMID, NULL);
}
