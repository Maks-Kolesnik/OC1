#include <stdio.h>
#include <stdlib.h>

#include "integral.h"


double func(double x) {
	return 4.0 - x*x;
}

int main() {
	IntegrArgs args = {5, 0.0, 2.0, 1.0e-6, func, N_THREAD_MODE};	
	double res = calcIntegral(&args);
	printf("Result: %lf\n", res);
	return 0;
}

