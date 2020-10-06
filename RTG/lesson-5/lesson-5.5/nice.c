//Listing 5.3 ( fork.c) Using fork to Duplicate a Program�s Process

/* Includes */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
 

/*Questions
1. can you estimate the duty-cycle betweenn the processes
2. enable the nice() calls on lines 40 and 55  what happens ?

*/

/************************************************************************************/
int main()
{
    pid_t childpid; /* variable to store the child's pid */
    int retval;     /* child process: user-provided return code */
    int status;     /* parent process: child's exit status */
    int counter=0;
    long i,j;


        
    /* now create new process */
    childpid = fork();
    
    if (childpid >= 0) /* fork succeeded */
    {
        if (childpid == 0) /* fork() returns 0 to the child process */
        {
            printf("CHILD: I am the child process!\n");
            printf("CHILD: Here's my PID: %d\n", getpid());
            
            nice(-20);
	    
            for(;;)
	    {
               	printf ("CHILD child_counter = %d\n", counter++);
	    }
            //we"ll never get here but thats ok
            printf("CHILD: Goodbye!\n");    
            exit(retval); /* child exits with user-provided return code */
        }
        else /* fork() returns new pid to the parent process */
        {
            printf("PARENT: I am the parent process!\n");
            printf("PARENT: Here's my PID: %d\n", getpid());
	    
             nice(+20);
 	  
            for(;;)
	    {
               	printf ("PARENT parent_counter = %d\n", counter++);
            }
            
            //we"ll never get here but thats ok
	    printf("PARENT: Goodbye!\n");             
            exit(0);  /* parent exits */       
        }
    }
    else /* fork returns -1 on failure */
    {
        perror("fork"); /* display error message */
        exit(0); 
    }
}


/*
Answers
1. only 1 int variable is needed because each process would have its
   own instance of the variable 
2. discuss in class
*/
