#ifndef TASK_H_
#define TASK_H_


#include <unistd.h>
#include "buffer.h"

typedef struct {
	int num_readers;
	int num_writers;
	int num_reps;
	int work_time;
	BUFFER_STRUCT * buf;
} THREAD_CONFIG;

void task_solution(THREAD_CONFIG * init);

#endif
