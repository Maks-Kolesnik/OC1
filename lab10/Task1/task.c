#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "task.h"
#include "threads.h"

void task_solve(PROG_STRUCT * init) {
	pthread_t * producers;
	pthread_t * consumers;	
	pthread_attr_t attr;
	int i;
	
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	
	producers = (pthread_t *) calloc(init->num_writers, sizeof(pthread_t));
	if (!producers) {
		printf("Memory error\n");
		exit(1);
	}
	for (i = 0; i < init->num_writers; i++) {
		if (pthread_create(&producers[i], &attr, producer, init->buf)) {
			printf("Creation Error\n");
			exit(1);
		}
	}
	
	consumers = (pthread_t *) calloc(init->num_readers, sizeof(pthread_t));
	if (!consumers) {
		printf("Consumers allocation memory error\n");
		exit(1);
	}
	for (i = 0; i < init->num_readers; i++) {
		if (pthread_create(&consumers[i], &attr, consumer, init->buf)) {
			printf("Creation Error\n");
			exit(1);
		}
	}
	pthread_attr_destroy(&attr);
	
	sleep(init->work_time);
	
	if (init->num_writers >= init->num_readers) {
		for (i = 0; i < init->num_writers; i++) {
			if (pthread_cancel(producers[i]) != 0) {
				printf("Canceling Error\n");
				exit(1);
			}
		}
		for (i = 0; i < init->num_writers; i++) {
			if (pthread_join(producers[i], NULL) != 0) {
				printf("Waiting Error\n");
				exit(1);
			}
		}
		printf("The Writer Thread was stopped\n");
		for (i = 0; i < init->num_readers; i++) {
			if (pthread_cancel(consumers[i]) != 0) {
				printf("Canceling Error\n");
				exit(1);
			}
		}
		
		for (i = 0; i < init->num_readers; i++) {
			if (pthread_join(consumers[i], NULL) != 0) {
				printf("Waiting Error\n");
				exit(1);
			}
		}
		printf("The Reader Thread was stopped\n");
	} else {
		for (i = 0; i < init->num_readers; i++) {
			if (pthread_cancel(consumers[i]) != 0) {
				printf("Canceling Error\n");
				exit(1);
			}
		}
		for (i = 0; i < init->num_readers; i++) {
			if (pthread_join(consumers[i], NULL) != 0) {
				printf("Waiting Error\n");
				exit(1);
			}
		}
		printf("The Reader Thread was stopped!\n");
		for (i = 0; i < init->num_writers; i++) {
			if (pthread_cancel(producers[i]) != 0) {
				printf("Canceling Error\n");
				exit(1);
			}
		}
		
		for (i = 0; i < init->num_writers; i++) {
			if (pthread_join(producers[i], NULL) != 0) {
				printf("Waiting Error\n");
				exit(1);
			}
		}
		printf("\nThe Writer thread was stopped!!\n");
	}

	free(producers);
	free(consumers);
}
