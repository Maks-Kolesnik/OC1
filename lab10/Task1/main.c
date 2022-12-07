#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "task.h"

int main(void) {
	printf("Program starts. Semaphores Task\n");
	{
		PROG_STRUCT prog;
		BUFF_STRUCT buf;
		
		init(&buf, 10);
		
		prog.buf = &buf;
		prog.num_readers = 3;
		prog.num_writers = 3;
		prog.work_time = 10;
		task_solve(&prog);
		
		dest(&buf);
	}
	printf("\n\tProgram stopped\n");
	return 0;
}

