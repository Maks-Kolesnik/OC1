#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "threads.h"
#include "buffer.h"

#define MAX 10

void *producer(void *arg) {
	int item;
	int old_cancel_state;
	BUFF_STRUCT * buff = (BUFF_STRUCT *) arg;
	
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	srand(time(NULL));
	while(1) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_cancel_state);
		item = rand() % MAX;
		set(buff, item);
		pthread_setcancelstate(old_cancel_state, NULL);
		pthread_testcancel();
		sleep(1);
	}
	return NULL;
}

void *consumer(void *arg) {
	int item;
	int old_cancel_state;
	BUFF_STRUCT * buff = (BUFF_STRUCT *) arg;
	
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	while(1) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_cancel_state);
		item = get(buff);
		pthread_setcancelstate(old_cancel_state, NULL);
		pthread_testcancel();
		sleep(1);
	}
	return NULL;
}
