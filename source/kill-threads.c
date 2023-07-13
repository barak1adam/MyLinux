
/* signals between threads using pthread_kill

Note: when compiling and link this program be sure to add the -lpthread library:
as the following:

% gcc kill-threads.c -lpthread -ggdb -o kill-threads */

/*************  includes     *****************/
#include <pthread.h>
#include <stdio.h>
#include <signal.h> //signals
#include <unistd.h> //sleep
#include <stdlib.h> //exit

/*************  Prototypes   *****************/
void* threadfunc();

/*************  main() function ****************/
// The main program.
int main ()
{
 int          status;
 pthread_t    threadid;

 status = pthread_create( &threadid,
                          NULL,
                          threadfunc,
                          NULL);
 if ( status <  0) {
    perror("pthread_create failed");
    exit(1);
 }

 sleep(5);

 status = pthread_kill( threadid, SIGUSR1);
 if ( status <  0)
    perror("pthread_kill failed");

 status = pthread_join( threadid, NULL);
 if ( status <  0)
    perror("pthread_join failed");

 exit(0);
}

void* threadfunc ()
{
    sigset_t   set;
    int sig = 0;
    int ret = 0;

    printf("Inptut Thread executing\n");
    sigemptyset(&set);
    if(sigaddset(&set, SIGUSR1) == -1) {
        perror("Sigaddset error");
        pthread_exit((void *)1);
    }
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    do {
        ret = sigwait(&set, &sig);
        perror("perr");
        if(ret) {
            perror("Sigwait error");
            pthread_exit((void *)2);
        }
        printf("waiting \n\r");
        if (sig == SIGUSR1)
            printf("Input Thread got SIGUSR1 successfully");
    } while (ret);

    printf("Inptut Thread terminating\n");
    pthread_exit((void *)0);
}



