#ifndef THREADS_H_
#define THREADS_H_

#include "buffer.h"

typedef struct {
	int num_thread;
	int num_reps;
	BUFFER_STRUCT * buf;
} TARG;

void * detach_thread(void * arg);
void * producer_thread(void * arg);
void * consumer_thread(void * arg);

#endif
