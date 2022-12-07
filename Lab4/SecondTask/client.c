#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <string.h>
#include <signal.h>

#define SIZE 255

int main(int argc, char **argv) {
	char name1[] = "fl1.fifo";
	char name2[] = "fl2.fifo";
	char buffer[SIZE];
	int fd1, fd2;
	pid_t pid;
	
	
	if (argc != 2) {
		printf("Wrong argument number!!!\n");
		exit(1);
	}
	
	printf("\nClient starts work");
	
	pid = (pid_t)atoi(argv[1]);

	printf("\n%d\n", pid);
	
	if (((fd1 = open(name1, O_RDONLY)) < 0) || ((fd2 = open(name2, O_WRONLY)) < 0)) {
		
		printf("\nCan`t open fifo file!!!\n");\
		exit(EXIT_FAILURE);
	}
	
	printf("\nEnter message (Max length %d characters)", SIZE);

	while (1) {
		printf("\n('exit' to stop)\n-> ");

		fgets(buffer, SIZE, stdin);

		buffer[strlen(buffer) - 1] = '\0';
		
		if (!strcmp(buffer, "exit")) break;
		
		write(fd2, buffer, strlen(buffer) + 1);
		
		read(fd1, buffer, SIZE);
		
		printf("\nGet message: %s", buffer);
	}
	
	close(fd2);  
	close(fd1);
	
	kill(pid, SIGTERM);
	
	printf("\nClient finished\n");
	
	return EXIT_SUCCESS;
}
