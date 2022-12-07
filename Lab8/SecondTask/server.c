#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define SIZE 255

sig_atomic_t flag = 1;

void handler(int signo) {
	if (signo == SIGTERM) flag = 0;
}


int main(void) {
	struct sigaction act; 
	char name1[] = "fl1.fifo";
	char name2[] = "fl2.fifo";
	char buffer[SIZE];
	int fd1, fd2, i;
	
	fprintf(stderr, "\nServer (PID: %d) starts work", (int)getpid());

	sigfillset(&act.sa_mask);
	act.sa_handler = handler;
	
	sigaction(SIGTERM, &act, NULL);
	signal(SIGPIPE, SIG_IGN);
	
	(void)umask(0);
	
	if ((mkfifo(name1, 0666) < 0) && (errno != EEXIST)) {
		printf("\nCan`t create fifo file\n");
		exit(1);
	}
	if ((mkfifo(name2, 0666) < 0) && (errno != EEXIST)) {
		printf("\nCan`t create fifo file\n");
		exit(1);
	}
	
	if (((fd1 = open(name1, O_WRONLY)) < 0) || ((fd2 = open(name2, O_RDONLY)) < 0)) {
		printf("\nCan`t open fifo file\n");
	
	}
	
	while (flag) {
		if((read(fd2, buffer, SIZE) || !flag) == 0) break;
		
		printf("\n\tServer get message: %s", buffer);
		
		for (i = 0; buffer[i]; i++) {
			buffer[i] = toupper(buffer[i]);
		}
		
		printf("\n\tServer transformed message: %s", buffer);

		write(fd1, buffer, strlen(buffer) + 1);
	}
	
	close(fd1);
	close(fd2);
	
	if ((unlink(name1) < 0) || (unlink(name2) < 0)) {
		printf("Cannot delete fifo1 file");
	}
	
	printf("\nServer finished\n");
	
	return 0;
}
