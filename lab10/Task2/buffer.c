#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

void initializer(BUFFER_STRUCT * buf, int num) {
	int res1, res2, res3;
	buf->buffer = (double *)calloc(num, sizeof(double));
	if (!buf->buffer) {
		printf("Buffer Allocation error");
		exit(1);
	}
	buf->size = num;
	buf->ind = -1;
	res1 = pthread_mutex_init(&(buf->lock), NULL);
	res2 = pthread_cond_init(&(buf->fill), NULL);
	res3 = pthread_cond_init(&(buf->empty), NULL);
	if (res1 + res2 + res3) {
		printf("Initialization Failed!");
		exit(1);
	}
}

void destructor(BUFFER_STRUCT * buf) {
	int res1, res2, res3;
	free(buf->buffer);
	buf->buffer = NULL;
	buf->size = 0;
	buf->ind = -1;
	res1 = pthread_mutex_destroy(&(buf->lock));	
	res2 = pthread_cond_destroy(&(buf->fill));
	res3 = pthread_cond_destroy(&(buf->empty));
	if (res1 + res2 + res3) {
		printf("De-Initialization Failed!");
		exit(1);
	}
}


double get(BUFFER_STRUCT * buf) {
	double tmp;
	
	pthread_mutex_lock(&(buf->lock));
	while(buf->ind == -1) {
		pthread_cond_wait(&(buf->fill), &(buf->lock));
	}
	
	tmp = buf->buffer[buf->ind];
	buf->buffer[buf->ind] = 0.0;
	buf->ind -= 1;
	
	pthread_cond_signal(&(buf->empty)); 
	pthread_mutex_unlock(&(buf->lock)); 
	
	return tmp;
}


void show(BUFFER_STRUCT * buf, FILE ** fptr) {
	pthread_mutex_lock(&(buf->lock));
	{
		int i;
		for(i = 0; i < buf->size/*ind*/; i++) {
			fprintf(*fptr, "%9.4f", buf->buffer[i]);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&(buf->lock));
}

void put(BUFFER_STRUCT * buf, double value) {
	pthread_mutex_lock(&(buf->lock));
	while(buf->ind == buf->size-1) {
		pthread_cond_wait(&(buf->empty), &(buf->lock));
	}
	buf->ind += 1;
	buf->buffer[buf->ind] = value;
	pthread_cond_signal(&(buf->fill));  
	pthread_mutex_unlock(&(buf->lock));
}



