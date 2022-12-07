#include <stdio.h>
#include <stdlib.h>

#include "RWTest.h"

int main(int argc, char** args) {
	int readersCount, writersCount, size;
	if(argc != 4){
		readersCount = 3;
		writersCount = 2;
		size = 10;
	} else {
		readersCount = atoi(args[1]);
		writersCount = atoi(args[2]);
		size = atoi(args[3]);
	}
	
	ProgConf configs;
	ThreadArray arr;

	if (!(arr.res = (double *)malloc(size * sizeof(double)))) {
		printf("Error while allocating memory array!");
	}
	
	arr.size = size;
	
	if(pthread_rwlock_init(&(arr.lock), NULL)) {
		printf("Error while initializing rwlock!");
	}
	
	configs.arr = &arr;
	configs.readersCount = readersCount;
	configs.writersCount = writersCount;
	RWTesting(&configs);
		
	free(arr.res);
	arr.res = NULL;
	pthread_rwlock_destroy(&(arr.lock));
	
	return 0;
}
