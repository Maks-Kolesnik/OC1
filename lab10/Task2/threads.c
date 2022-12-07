#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "threads.h"
#include "buffer.h"

void * detach_thread(void * arg) {
	BUFFER_STRUCT buf = *(BUFFER_STRUCT*)arg;
	
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) {
		printf("Cannot change cancel state\n");
		exit(1);
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)) {
		printf("Cannot change cancel type\n");
		exit(1);
	}
	
	while (1) {
	
		show(&buf, &stdout);
		sleep(1);
	}

	return NULL;
}


void * producer_thread(void * arg) {
	TARG * targ = (TARG *)arg;
	int k, ind;
	double val;
	printf("Writer Thread %d is started\n", targ->num_thread);
	sleep(targ->num_thread);
	srand(time(NULL));
	for (k = 0; k < targ->num_reps; k++) {
		val = (double)rand() / RAND_MAX;
		put(targ->buf, val);
		printf("Writer Thread %d, result = %g\n", targ->num_thread, val);
		sleep(rand() % targ->buf->size);
	}
	printf("Writer Thread %d is stopped\n", targ->num_thread);
	return NULL;
}


void * consumer_thread(void * arg) {
	TARG * targ = (TARG *)arg;
	int k, ind;
	double res;
	printf("\t\tReader Thread %d is started\n", targ->num_thread);
	sleep(targ->num_thread);
	srand(time(NULL));
	for (k = 0; k < targ->num_reps; k++) {
		res = get(targ->buf);
		printf("Reader Thread %d, result = %g\n", targ->num_thread, res);
		
		sleep(rand() % targ->buf->size);
	}
	printf("Reader Thread %d is stopped\n", targ->num_thread);
	return NULL;
}
