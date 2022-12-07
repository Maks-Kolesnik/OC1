#define TASK_H_

#include "buffer.h"

typedef struct {
	int num_readers;
	int num_writers;
	int work_time;
	BUFF_STRUCT * buf;
} PROG_STRUCT;

void task_solve(PROG_STRUCT * init);

