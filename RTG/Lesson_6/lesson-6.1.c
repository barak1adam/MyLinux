/* Home work solution 6.1 

Description
-----------


Note: when compiling and link this program be sure to add the -lpthread library:
as the following:

% gcc join-threads.c -lpthread -ggdb -o join-threads

/*************  includes     *****************/
/*
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
*/
#include <pthread.h>
#include <stdio.h>

/*************  Defines     *****************/
#define NUM_OF_THREADS 100


/*************  Prototypes   *****************/
void* thread_func();

/********s* ****  Structures   *****************/
// Parameters to print_function. 
struct thread
{
	pthread_t threads[NUM_OF_THREADS];    // array of thread ID's
	int thread_count;                     //  number of threads created
};

/*************  Globals   *****************/
struct thread th;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
/*************  main() function ****************/ 
// The main program. 
int main ()
{
	int i=0;
	th.thread_count=0;
	
	for(i=0;i<NUM_OF_THREADS;i++)
	{
		pthread_create (&th.threads[i], NULL, &thread_func, NULL);
	}
	
	for(i=0;i<NUM_OF_THREADS;i++)
	{
		pthread_join (th.threads[i], (void*) NULL);
  	}

	return 0;
}

/*************  char_print() function ****************/
void* thread_func ()
{
	int i=0;
	pthread_mutex_lock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	th.thread_count++;
	printf(" num of threads %d\n",th.thread_count);
	for(i=0;i<th.thread_count;i++)
	{
		printf(" thread id %d 0x%x \n",i,th.threads[i]);
	}
	printf(" \n \n \n");
	pthread_mutex_unlock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	return;
}



