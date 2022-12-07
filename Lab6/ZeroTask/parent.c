#include <wait.h>
#include <stdio_ext.h>
#include <string.h>

#include "lib.h"

int getUINT(int max) {
	int num;
	char *str = (char*) malloc(64 * sizeof(char));
	
	do {
		printf("\nEnter number between 1 and %d or -1 to exit: ", max);
		
		__fpurge(stdin);
		fgets(str, 64, stdin);

		if (!strcmp(str,"-1\n")) return -1;

		num = atoi(str);

		if (num == 0 || num < 0 || num > max) printf("\nIncorrect input!\n");
	} while (num == 0 || num < 0 || num > max);

	return num;
}

int main(int argc, char **args) {
	int bufSize, size, i;
	int *arr;
	pid_t pid;
	SHMType *shm = NULL;
	char buffer[16];

	if (argc != 2) {
		bufSize = 10;
	}
	else {
		bufSize = atoi(args[1]);
	}

	shm = createSHM(bufSize);

	if ((pid = fork()) == -1) {
		printf("\nError! Can`t create child process!\n");
		exit(EXIT_FAILURE);
	
	} else if (pid == 0) {
		sprintf(buffer, "%d", shm->shm_id);

		printf("\nParent process start child process\n");
		execlp("./child", "child", buffer, NULL);
	
	} else {
		sleep(1);

		arr = (int*) malloc(bufSize * sizeof(int));
		
		while (1) {
			printf("\nEnter array size: ");
			if ((size = getUINT(bufSize - 1)) == -1) {
				setBuffer(shm, NULL, 0);
				break;
			}

			printf("Input %d integers\n", size);
			for (i = 0; i < size; i++) {
				printf("arr[%d] = ", i);
				scanf("%d", &arr[i]);
			}

			setBuffer(shm, arr, size);

			printf("\nResult: %d\n", getResult(shm));
		}

		free(arr);

		kill(pid, SIGTERM);
		wait(NULL);

		deleteSHM(shm);
	}

	printf("\nParent process finished\n");
	
	return EXIT_SUCCESS;
}
