#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdio.h>
#include <pthread.h>

typedef struct {
	double *buffer;
	int size;
	int ind;
	pthread_mutex_t lock;
	pthread_cond_t empty;
	pthread_cond_t fill;
} BUFFER_STRUCT;

void initializer(BUFFER_STRUCT * buf, int num);
void destructor(BUFFER_STRUCT * buf);
void show(BUFFER_STRUCT * buf, FILE ** fptr);
void put(BUFFER_STRUCT * buf, double value);
double get(BUFFER_STRUCT * buf);

#endif
