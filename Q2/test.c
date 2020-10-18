// C program to implement cond(), signal() 
// and wait() functions 
#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 

// Declaration of thread condition variable 
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 

// declaring mutex 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

int done = 1; 

// Thread function 
void* foo(void* inp) 
{ 
    int* s = (int*)inp;

	// acquire a lock 
	pthread_mutex_lock(&lock);
    printf("locked by %d\n",*s); 
	if (done == 1) { 

		// let's wait on conition variable cond1 
		done = 2; 
		printf("Waiting on condition variable cond1 by %d\n",*s); 
		pthread_cond_wait(&cond1, &lock); 
	} 
	else { 

		// Let's signal condition variable cond1 
		printf("Signaling condition variable cond1 by %d\n",*s); 
		pthread_cond_signal(&cond1); 
	} 

	// release lock 
    printf("unlocked by %d\n",*s);
	pthread_mutex_unlock(&lock); 

	printf("Returning thread %d\n",*s); 

	return NULL; 
} 

// Driver code 
int main() 
{ 
	pthread_t tid1, tid2; 

	// Create thread 1 
    printf("seg1\n");
    int s1 = 1;
    int s2 = 2;

    printf("seg\n");
	pthread_create(&tid1, NULL, foo, (void*)&s1); 

	// sleep for 1 sec so that thread 1 
	// would get a chance to run first 
	sleep(1); 

	// Create thread 2 
	pthread_create(&tid2, NULL, foo, (void*)&s2); 

	// wait for the completion of thread 2 
	pthread_join(tid2, NULL); 
    pthread_join(tid1,NULL);

	return 0; 
} 
