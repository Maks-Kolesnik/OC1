#include "lib.h"

int flag = 0;

void handler(int signo) {
	flag = 1;
}

int main(int argc, char **args) {
	int shm_id;
	SHMType* shm;

	printf("\nChild process started\n");
	
	if (argc != 2) {
		printf("\nError: Incorrect number of arguments\n");
		exit(1);
	}

	signal(SIGTERM, handler);

	shm_id = atoi(args[1]);
	shm = (SHMType*)shmat(shm_id, NULL, 0);

	if (shm == (SHMType*)-1) {
		printf("\nError! Can`t connect to shared memory\n");
		exit(1);
	}

	while(!flag) {
		calculationSum(shm);
	}

	shmdt(shm);

	printf("\nChild process finished\n");
	
	return 0;
}
