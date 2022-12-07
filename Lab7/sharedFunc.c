#include "lib.h"

#include <string.h>

int getSHM_ID(int size, int mode) {
	int bufSize = 0, flags = 0, shm_id;
	key_t shm_key;
	
	if ((shm_key = ftok("./sharedMemory", 0)) == -1) {
		printf("\nError! Can`t get key for shared memory\n");
		exit(EXIT_FAILURE);
	}

	if (mode == SERVER) {
		bufSize = size;
		flags = 0200 | 0400 | IPC_CREAT | IPC_EXCL;
	}

	if ((shm_id = shmget(shm_key, bufSize, flags)) == -1) {
		printf("\nError! Can`t get shared memory id\n");
		exit(EXIT_FAILURE);
	}

	return shm_id;
}

void *getDataFromSHM(int size, int *shm_id, int mode) {
	void* shm_buf = NULL;
	*shm_id = getSHM_ID(size, mode);
	
	if ((shm_buf = shmat(*shm_id, NULL, 0)) == (void*)-1) {
		printf("\nError! Can`t get data from shared memory\n");
		exit(EXIT_FAILURE);
	}

	return shm_buf;
}

void disconnectSHM(void* shm_buf, int shm_id, int mode) {
	if (shmdt(shm_buf) == -1) {
		printf("\nError! Can`t disconnect shared memory\n");
		exit(EXIT_FAILURE);
	}

	if (mode == SERVER) {
		if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
			printf("\nError! Can`t delete shared memory\n");
			exit(EXIT_FAILURE);
		}
	}
}

int* getSEM_ID(int mode) {
	int* sem_id = (int*) malloc(2 * sizeof(int));
	int flags = 0;
	key_t* sem_keys = (key_t*)malloc(2 * sizeof(key_t));
	
	if ((sem_keys[WORK_SEM] = ftok("./semaphore1", 0)) == -1) {
		printf("\nError! Can`t get file descriptor for semaphores\n");
		exit(EXIT_FAILURE);
	}

	if ((sem_keys[WRITE_SEM] = ftok("./semaphore2", 0)) == -1) {
		printf("\nError! Can`t get file descriptor for semaphores\n");
		exit(EXIT_FAILURE);
	}

	if (mode == SERVER) {
		flags = 0200 | 0400 | IPC_CREAT | IPC_EXCL;
	}

	if ((sem_id[WORK_SEM] = semget(sem_keys[WORK_SEM], 1, flags)) == -1) {
		printf("\nError! Can`t creat or connect to semaphores\n");
		exit(EXIT_FAILURE);
	}

	if ((sem_id[WRITE_SEM] = semget(sem_keys[WRITE_SEM], 1, flags)) == -1) {
		printf("\nError! Can`t creat or connect to semaphores\n");
		exit(EXIT_FAILURE);
	}

	if (mode == SERVER) {
		semun arg;
		arg.val = 0;
		if ((semctl(sem_id[WORK_SEM], 0, SETVAL, arg)) == -1) {
			printf("\nError! Can`t change semaphores\n");
			exit(EXIT_FAILURE);
		}

		arg.val = 1;
		if ((semctl(sem_id[WRITE_SEM], 0, SETVAL, arg)) == -1) {
			printf("\nError! Can`t change semaphores\n");
			exit(EXIT_FAILURE);
		}
	}

	return sem_id;
}

void deleteSEM(int* sems_id) {
	if ((semctl(sems_id[WORK_SEM], 0, IPC_RMID, 0)) == -1 || (semctl(sems_id[WRITE_SEM], 0, IPC_RMID, 0)) == -1) {
		printf("\nError! Can`t disconnect semaphores\n");
		exit(EXIT_FAILURE);
	}
}
