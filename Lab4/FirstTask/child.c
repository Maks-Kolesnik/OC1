#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/shm.h>

int exitFlag = 1;
int execFlag = 0;

void childHandler(int sig) { 
	if (sig == SIGTERM) exitFlag = 0;
	if (sig == SIGUSR1) execFlag = 1;
}

void setSignalMask() {
	sigset_t signalMask;

	sigemptyset(&signalMask);
	sigaddset(&signalMask, SIGUSR1);
	sigaddset(&signalMask, SIGTERM);

	if (sigprocmask(SIG_BLOCK, &signalMask, NULL) != 0) {
		printf("Cannot set signals mask\n");
		exit(1);
	}
}

int main(int argc, char **argv) {
	sigset_t set;
	struct sigaction action;
	int shm_id, i;
	int* shm_buf = NULL;

	if (argc != 2) {
		printf("Wrong number of arguments\n");
		exit(1);
	}
	
	shm_id = atoi(argv[1]);

	printf("\nChild process is started");

	sigfillset(&set);
	sigdelset(&set, SIGUSR1);
	sigdelset(&set, SIGTERM);
  
	setSignalMask();
  
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESTART;
	action.sa_handler = childHandler;
	
	if (sigaction(SIGUSR1, &action, NULL) != 0  || sigaction(SIGTERM, &action, NULL) != 0) {
		printf("\nError! Can`t config signals mask\n");
		exit(1);
	}

	if ((shm_buf = (int*) shmat(shm_id, 0, 0)) == (int*) -1) {
		printf("Error! Can`t get data from shared memory\n");
		exit(1);
	}

	kill(getppid(), SIGUSR1);

	while(exitFlag) {
		if (sigsuspend(&set) == -1 && errno != EINTR) {
			printf("\nError waiting SIGUSR1\n");
			exit(1);
		}
		
		if (execFlag) {
			printf("\nChild process start work");
			int size = shm_buf[0], sum = 0;
			
			shm_buf[0] = 0;
			
			for (i = 1; i <= size; i++) {
				sum += shm_buf[i];
				shm_buf[i] = 0;
			}
			
			shm_buf[0] = sum;
			execFlag = 0;
		}
		
		kill(getppid(), SIGUSR1);
	}
	
	shmdt(shm_buf);
	printf("\nChild process finished\n");
  
	return 0;
}
