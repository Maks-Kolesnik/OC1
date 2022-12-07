#include "lib.h"

#include <stdio_ext.h>
#include <string.h>

int firstItrFlag = 1;

int getUINT() {
	int num;
	char *str = (char*) malloc(64 * sizeof(char));
	
	do {
		printf("\nEnter number more 1 or `-1` to exit: ");
		
		__fpurge(stdin);
		fgets(str, 64, stdin);

		if (!strcmp(str,"-1\n")) return -1;

		num = atoi(str);

		if (num == 0 || num < 0) printf("\nIncorrect input!\n");
	} while (num == 0 || num < 0);

	return num;
}

int fillBuf(int* shm_buf) {
	int size, i;
	
	if (firstItrFlag == 0) {
		printf("\nCalculation result: %d\n", shm_buf[0]);
	}
	else {
		firstItrFlag = 0;
	}
	
	printf("\nEnter array size");
	if ((size = getUINT()) == -1) return EXIT_SUCCESS;

	shm_buf[0] = size;

	printf("Input %d integers\n", size);
	for (i = 1; i <= size; i++) {
		printf("arr[%d] =  ", i);
		scanf("%d", &shm_buf[i]);
	}
}

int main(int argc, char **argv) {
	int shm_id;
	int *buf, *sems;

	printf("\nClient Process is started");
	
	buf = (int*) getDataFromSHM(0, &shm_id, CLIENT);
	sems = getSEM_ID(CLIENT);

	printf("\nClient start work");
	
	while(1) {
		if (semop(sems[WRITE_SEM], &waitBuf, 1) == -1) {
			printf("\nError! Can`t change semaphores\n");
			exit(EXIT_FAILURE);
		}
		
		if (fillBuf(buf) == EXIT_SUCCESS) break;
	
		
		if (semop(sems[WORK_SEM], &postBuf, 1) == -1) {
			printf("\nError! Can`t change semaphores\n");
			exit(EXIT_FAILURE);
		}
	}

	disconnectSHM(buf, shm_id, CLIENT);

	printf("\nClient process is finished\n");

	return EXIT_SUCCESS;
}
