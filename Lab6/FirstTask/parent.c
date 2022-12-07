#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio_ext.h>
#include <string.h>
#include <wait.h>

int flag = 0;

void handler(int signo) { 
	if (signo == SIGUSR1) flag = 1;
}

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

void setSignalMask() {
	sigset_t signalMask;

	sigemptyset(&signalMask);
	sigaddset(&signalMask, SIGUSR1);

	if (sigprocmask(SIG_BLOCK, &signalMask, NULL) != 0) {
		printf("\nError! Can`t set signal mask\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **args) {
	struct sigaction action;
	int shm_id, size, i;
	sigset_t set;
	pid_t pid;
  
	printf("\nParent process started\n");

	sigfillset(&set);
	sigdelset(&set, SIGUSR1);
	
	setSignalMask();
	
	sigemptyset(&action.sa_mask);
	
	action.sa_flags = SA_RESTART;
	action.sa_handler = handler;
	
	if (sigaction(SIGUSR1, &action, NULL) != 0) {
		printf("\nError! Can`t config signal mask for parent process\n");
		exit(EXIT_FAILURE);
	}

	if (argc != 2) {
		size = 10;
	} else {
		size = atoi(args[1]);
	}

	shm_id = shmget(IPC_PRIVATE, size * sizeof(int), IPC_CREAT | IPC_EXCL | 0200 | 0400);

	if (shm_id == -1) {
		printf("\nError! Can`t creat or add to process shared memory!\n");
		exit(EXIT_FAILURE);
	}

	if ((pid = fork()) == -1) {
		printf("\nError! Can`t create child process\n");
		exit(EXIT_FAILURE);
		
	} else if(pid == 0) {  
		char temp[16];
		sprintf(temp, "%d", shm_id);

		printf("\nParent starting child program");
		
		execlp("./child", "child", temp, NULL);
		
	} else {
		int *shm_buf = (int*) shmat(shm_id, NULL, 0);

		if (shm_buf == (int*) -1) {
			printf("\nError! Can`t connection to shared memory\n");
			exit(EXIT_FAILURE);
		}

		shm_buf[0] = 0;
		
		while (1) {
			int temp;
			if (sigsuspend(&set) == -1 && errno != EINTR) {
				printf("\nError waiting SIGUSR1\n");
				exit(EXIT_FAILURE);
			}

			printf("\nParent received signal");

			int sum = shm_buf[0];
			printf("\nSum: %d", sum);

			printf("\nEnter array size: ");
			if ((temp = getUINT(size - 1)) == -1) break;

			shm_buf[0] = temp;

			printf("Input %d integers\n", temp);
			for (i = 1; i <= temp; i++) {
				printf("arr[%d] = ", i);
				scanf("%d", &shm_buf[i]);
			}
			
			kill(pid, SIGUSR1);
		}
		
		kill(pid, SIGTERM);
		wait(NULL);
		
		shmdt(shm_buf);
		shmctl(shm_id, IPC_RMID, NULL);
	}
  
	return 0;
}
