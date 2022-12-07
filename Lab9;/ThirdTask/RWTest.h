#include <pthread.h>

typedef struct {
	double * res;
	int size;
	pthread_rwlock_t lock;
} ThreadArray;

typedef struct {
	int readersCount;
	int writersCount;
	int reps;
	ThreadArray* arr;
} ProgConf;

void RWTesting(ProgConf* init);
