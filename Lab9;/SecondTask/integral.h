#define ONE_THREAD_MODE 1
#define N_THREAD_MODE 2

typedef struct {
	int n;
	double a, b, eps;
	double (*f)(double);
	int mode;
} IntegrArgs;

double calcIntegral(const IntegrArgs* arg);
