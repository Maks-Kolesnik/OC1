#ifndef BUFFER_H_

#define BUFFER_H_

#include <semaphore.h>

typedef struct {
	int size;
	int *buff;
	int index;
	sem_t empty_items;
	sem_t full_items; 
	sem_t lock;
} BUFF_STRUCT;

void init(BUFF_STRUCT * buff, int size);
void dest(BUFF_STRUCT * buff);
void set(BUFF_STRUCT * buff, int elem);
int get(BUFF_STRUCT * buff);

#endif
