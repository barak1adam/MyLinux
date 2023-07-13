/* valgrind_nir.c
 * This is a simple example of a program which creating 2 threads.
 * First thread is allocating a block of memory on the heap.
 * Second thread is calling free for the block but not assign a NULL to the ptr
 * First thread is trying to access the laready free'd memory because it is not NULL...
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


pthread_t tid[2];

void test_size()
{
	/*char array[(8* 1024 * 1024)+100];
	array[(8* 1024 * 1024) +50] = 'n';*/
}
void* doSomeThing(void *arg)
{
    int i;
    pthread_t id = pthread_self();
    pthread_attr_t attr;
    int err = 0;
    size_t stack_size;

    err = pthread_attr_init(&attr);
    if (err != 0)
        printf("pthread_attr_init failed");

    if(pthread_equal(id,tid[0]))
    {
        pthread_attr_getstacksize(&attr, &stack_size);
        printf("\n First thread processing %d\n", (int)stack_size);
    }
    else
    {
        pthread_attr_getstacksize(&attr, &stack_size);
        printf("\n Second thread processing %d\n", (int)stack_size);
    }


	for(i=0; i<(0xFFFFFFFF);i++);

	test_size();

	err = pthread_attr_destroy(&attr);
    if (err != 0)
       printf("pthread_attr_destroy failed");

    return NULL;
}


int main(void)
{
    int i = 0;
    int err;
    pthread_attr_t attr;
    //int stack_size = 0x2000;

    err = pthread_attr_init(&attr);
    if (err != 0)
        printf("pthread_attr_init failed");

	err = pthread_attr_setstacksize(&attr, 8*1024*1024);
    if (err != 0)
        printf("pthread_attr_setstacksize failed");

    while(i < 2)
    {
        err = pthread_create(&(tid[i]), &attr, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread failed");
        else
             printf("\n Thread created successfully\n");

        i++;
    }

    err = pthread_attr_destroy(&attr);
    if (err != 0)
       printf("pthread_attr_destroy failed");

     return 0;
}
