 /* Listing 6.6-4 ( cancel-thread4.c) 

Description
-----------


Note: when compiling and link this program be sure to add the -lpthread library:
as the following:

% gcc cancel-thread_4.c -lpthread -o a.out


 >ulimit -s
8192 - meaning the zise is 8 Mbyte
or in code

- RLIMIT_STACK can in no case be exceeded?
this is a very good question
please run the following code

please run the following:
$ ulimit -s
8192
$ ./a.out 
8388608
8388608
$ ulimit -s unlimited
$ ./a.out 
-1
2097152
$ ulimit -s 4096
$ ./a.out 
4194304
4194304
do you understand whats going on ?
/*************  includes     *****************/
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>

/*************  Macros     *****************/
#define HANDLE_ERR(en, msg) \
               do { errno = en; perror(msg); \
			exit(EXIT_FAILURE);  \
		   } 			     \
		while (0)

int main()
{
    /* Warning: error checking removed to keep the example small */
    pthread_attr_t attr;
    size_t stacksize;
    struct rlimit rlim;

    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    getrlimit(RLIMIT_STACK, &rlim);
    /* Don't know the exact type of rlim_t, but surely it will
       fit into a size_t variable. */
    printf("%zd\n", (size_t) rlim.rlim_cur);
    printf("%zd\n", stacksize);
    pthread_attr_destroy(&attr);

    return 0; 
} 
