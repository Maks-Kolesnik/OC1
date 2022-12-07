#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>

typedef struct {
	int* buffer;
	int size;
	int shm_id;
	int shmBufferID;
	pthread_mutex_t lock;
	sem_t emptyBuffer;
	sem_t notEmptyBuffer;
	sem_t isResultInBuffer;
} SHMType;

SHMType* createSHM(int size);
void setBuffer(SHMType* shm, int* arr, int size);
void calculationSum(SHMType* shm);
int getResult(SHMType* shm);
void deleteSHM(SHMType* shm);
