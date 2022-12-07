#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <pthread.h>

#include "integral.h"

#define STACK_OVERFLOW 5

typedef struct {
	double * res;
	pthread_mutex_t *m;
	IntegrArgs arg;
} ThreadArg;

double integrate(const IntegrArgs* args, unsigned long n) {
	double res = 0, h = (args->b - args->a) / n;
	for (unsigned long i = 0; i < n; i++) {
		res += args->f(args->a + (i + 0.5)*h);
	}
	return res * h;
}

double IntegrateFunc(const IntegrArgs* args) {
	double i0, i1, difference = DBL_MAX;
	
	i0 = integrate(args, (unsigned long)10);
	for(unsigned long n = 20; difference >= args->eps; n *= 2){
		i1 = integrate(args, n);
		difference = fabs(i0 - i1);
		i0 = i1;
	}
	
	return i1;
}

void * TIntegrateFunc(void * arg) {
	ThreadArg data = *(ThreadArg*) arg;
	double res = IntegrateFunc(&data.arg);
	pthread_mutex_lock(data.m);
	*data.res += res;

	pthread_mutex_unlock(data.m);

	return NULL;
}


double ThreadIntegrate(const IntegrArgs* arg) {
	double h = (arg->b - arg->a) / arg->n , result = 0;
	pthread_mutex_t total_res_mutex;

	if (pthread_mutex_init(&total_res_mutex, NULL) != 0) {
		printf("Error while initializing mutex!\n");
		exit(1);
	}
	
	pthread_t *thread = (pthread_t*)malloc(arg->n * sizeof(pthread_t));
	if (!thread) {
		printf("Error while allocating memory!\n");
		exit(STACK_OVERFLOW);
	}
	
	ThreadArg *arr = (ThreadArg*)malloc(arg->n * sizeof(ThreadArg));
	if (!arr) {
		printf("Error while allocating memory!\n");
		exit(STACK_OVERFLOW);
	}
	
	for (int i = 0; i < arg->n; i++) {
		arr[i].res = &result;
		arr[i].m = &total_res_mutex;
		arr[i].arg.n = i;
		arr[i].arg.a = arg->a + i*h;
		arr[i].arg.b = arr[i].arg.a + h;
		arr[i].arg.eps = arg->eps;
		arr[i].arg.f = arg->f;

		if (pthread_create(&thread[i], NULL, &TIntegrateFunc, &arr[i]) != 0) {
			printf("Error while creating %d tread\n", i);
			exit(0);
		}
	}
	
	for (int i = 0; i < arg->n; i++) {
		if (!pthread_equal(pthread_self(), thread[i])){
			if (pthread_join(thread[i], NULL) != 0) {
				printf("Error while joining %d thread\n", i);
				exit(0);
			}
		}
	}
	
	pthread_mutex_destroy(&total_res_mutex);
	free(thread); 
	free(arr);
	return result;
}

double calcIntegral(const IntegrArgs* arg){
	if(arg->mode == ONE_THREAD_MODE){
		return IntegrateFunc(arg);
	} else if (arg->mode == N_THREAD_MODE){
		return ThreadIntegrate(arg);
	} else {
		return 0;
	}
}

