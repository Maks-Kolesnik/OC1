#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct{
	int mode;
	int num;
} ThreadArgument;

long int value = 0;

void * func(void * arg) {
	ThreadArgument targum = *((ThreadArgument *)arg);
	for (long i = 0; i < targum.num; i++) {
		if(targum.mode == 0){
			value += 1;
		} else {
			value -= 1;
		}
	}
	return NULL;
}


int main(int argc, char** argv) {
	int countOfThreads;
	long n;
	
	if(argc != 3){
		countOfThreads = 1;
		n = 1000000;
	} else {
		countOfThreads = atoi(argv[1]);
		n = atol(argv[2]);
	}
	
	pthread_t* threads = (pthread_t*)malloc(2*countOfThreads*sizeof(pthread_t));
	
	printf("Before operations: %ld\n", value);
	
	ThreadArgument tArg1 = {0, n};
	ThreadArgument tArg2 = {1, n};
	
	for(int i = 0; i < countOfThreads*2; i += 2){
		if(pthread_create(&threads[i], NULL, func, (void*)&tArg1)) {
			printf("Error while creating thread!\n");
			exit(1);
		}
		if(pthread_create(&threads[i+1], NULL, func, (void*)&tArg2)) {
			printf("Error while creating thread!\n");
			exit(1);
		}
	}
	
	for(int i = 0; i < countOfThreads * 2; i++){
		pthread_join(threads[i], NULL);
	}
	
	printf("\nAfter operations: %ld\n", value);
	
	return 0;
}
