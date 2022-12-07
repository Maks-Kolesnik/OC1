#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "buffer.h"

void init(BUFF_STRUCT * buf, int size) {
	buf->buff = (int *)calloc(size, sizeof(int));
	if (buf->buff == NULL) {
		printf("Allocation Memory Error\n");
		exit(1);
	}
	buf->index = 0;
	if (sem_init(&(buf->lock), 0, 1) != 0) {
		printf("Critical Section Semaphore Init Error\n");
		exit(1);
	}
	if (sem_init(&(buf->empty_items), 0, size) != 0) {
		printf("Empty Buffer Semaphore Init Error\n");
		exit(1);
	}
	if (sem_init(&(buf->full_items), 0, 0) != 0) {
		printf("Full Buffer Semaphore Init Error\n");
		exit(1);
	}
}


int get(BUFF_STRUCT * buf) {
	int pos, item;
	sem_wait(&(buf->full_items)); 
	sem_wait(&(buf->lock));       
	pos = buf->index-1;
	item = buf->buff[pos];      
	buf->buff[pos] = 0;
	buf->index -= 1;
	sem_post(&(buf->lock));       
	sem_post(&(buf->empty_items)); 
	printf("\tConsumer(%u): buf[%d] = %d\n", (unsigned int)pthread_self(), pos, item);
}


void set(BUFF_STRUCT * buf, int elem) {
	int pos;
	sem_wait(&(buf->empty_items)); 
	sem_wait(&(buf->lock));        
	pos = buf->index;
	buf->buff[pos] = elem;       
	buf->index += 1;
	sem_post(&(buf->lock));        
	sem_post(&(buf->full_items)); 
	printf("Producer(%u): buf[%d] = %d\n", (unsigned int)pthread_self(), pos, elem);
}


void dest(BUFF_STRUCT * buf) {
	sem_destroy(&(buf->empty_items));
	sem_destroy(&(buf->full_items));
	sem_destroy(&(buf->lock));
	free(buf->buff);
	buf->buff = NULL;
	buf->index = 0;
}

