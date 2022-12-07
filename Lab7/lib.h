#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define WORK_SEM 0
#define WRITE_SEM 1

#define SERVER 2
#define CLIENT 3

typedef union {
	int val;
	struct semid_ds *buf;
	ushort *array;
} semun;

static struct sembuf postBuf = { 0, 1, SEM_UNDO };
static struct sembuf waitBuf = { 0, -1, SEM_UNDO };

void* getDataFromSHM(int size, int* shm_id, int mode);
void disconnectSHM(void* shm_buf, int shm_id, int mode);
int* getSEM_ID(int mode);
void deleteSEM(int* sems_id);