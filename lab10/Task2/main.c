#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"
#include "task.h"

int main(int argc, char **argv) {
	printf("\n");
	{
		THREAD_CONFIG init;
		BUFFER_STRUCT buf;
		initializer(&buf, 10);
		
		init.buf = &buf;
		init.num_readers = 5;
		init.num_writers = 6;
		init.num_reps = 75;
		init.work_time = 15;
		task_solution(&init);
		
		destructor(&buf);
	}
	printf("\nThe program is stopped\n");
	
	return 0;
}
