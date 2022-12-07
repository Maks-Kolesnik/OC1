#include <unistd.h>
#include <signal.h>

#include "lib.h"

int flag = 0;

void handler(int sig) {
	if (sig == SIGTERM) printf("\nServer catched SIGTERM signal!\n");
	else if (sig == SIGINT) printf("\nServer catched SIGINT signal!\n");
	flag = 1;
}

void funcSum(int* buf) {
	int i, size = buf[0], res = 0;

	for (i = 1; i <= size; i++) {
		res += buf[i];
		buf[i] = 0;
	}

	buf[0] = res;
	printf("\nSUM: %d\n", res);
}

int main(int argc, char **argv) {
	int size, shm_id;
	int *buf, *sem_id;

	if (argc != 2) {
		size = 100 * sizeof(int);
	}
	else {
		size = atoi(argv[1]) * sizeof(int);
	}
	
	printf("\nServer process is started! PID: %d\n", (int) getpid());
	
	signal(SIGTERM, &handler);
	signal(SIGINT, &handler);

	buf = (int*) getDataFromSHM(size, &shm_id, SERVER);

	sem_id = getSEM_ID(SERVER);
	
	printf("\nServer start work");
	
	while(!flag) {
		if (semop(sem_id[WORK_SEM], &waitBuf, 1) == -1) {
			printf("\nError! Can`t change semaphores\n");
			break;
		}

		printf("\nServer make calculation");
		funcSum(buf);

		if (semop(sem_id[WRITE_SEM], &postBuf, 1) == -1) {
			printf("\nError! Can`t change semaphores\n");
			break;
		}
	}

	disconnectSHM(buf, shm_id, SERVER);
	
	deleteSEM(sem_id);

	printf("\nServer process has finished\n");
	
	return EXIT_SUCCESS;
}
