#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#define SIZE 255

sig_atomic_t flag = 1;

void handler (int sig) {
	flag = 0;
}

int main(void) {
	char buffer[SIZE];
	int fd1[2], fd2[2], i;
	pid_t pid;
	
	if ((pipe(fd1) == -1) || (pipe(fd2) == -1)) {
		printf("\nCan`t create pipe!\n"); 
	}
	
	switch(pid = fork()) {
	case -1:
		printf("\nCan`t create sub-process!\n");
		exit(1);
		
	case 0:
		if (signal(SIGTERM, handler) == SIG_ERR) {
			printf("\nCan`t invoking signal\n");
		}
		
		close(fd1[0]);
		close(fd2[1]); 
	
		while (flag) {
			read(fd2[0], buffer, SIZE);
			
			if (!flag) break;
			printf("\tSub-process get message: %s\n", buffer);
		
			for (i = 0; buffer[i]; i++) {
				buffer[i] = toupper(buffer[i]);
			}
			
			printf("\tSub-process transformed message: %s\n", buffer);
			
			write(fd1[1], buffer, strlen(buffer) + 1);
		}
		
		close(fd1[1]);
		close(fd1[0]);
		
		printf("\tSub-process is stopped\n");
		break;
		
	default: 
	
		close(fd1[1]); 
		close(fd2[0]);
		
		printf("\nEnter message (Max length %d characters)", SIZE);
		
		while (1) {
			printf("\n('exit' to stop program)\n->");
			
			fgets(buffer, SIZE, stdin);
			buffer[strlen(buffer)-1] = '\0';
			
			if (!strcmp(buffer, "exit")) break;

			write(fd2[1], buffer, strlen(buffer) + 1);

			read(fd1[0], buffer, SIZE);
			printf("Process get message from sub-process: %s\n", buffer);
		}
		
		close(fd1[0]);
		close(fd2[1]);
		
		kill(pid, SIGTERM);
		
		wait(NULL);
	}

	return 0;
}
