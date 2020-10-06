// Listing 5.7 ( fork-wait.c) Using fork to Duplicate a Programs Processes, 
// the parent will wait for the child to end and only then it will exit as well.



/*
Questions
---------
1. what happens when u enable the remarked lines in the child processes.
2. Why is it important, what might happen when the lines are remarked?
3. please use the waitpid() system call instead of the wait(), 
   add the necessary systems calls as well.


*/

/* Includes */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
 

/************************************************************************************/
int main()
{
    pid_t childpid; /* variable to store the child's pid */
    int retval;     /* child process: user-provided return code */
    int status;     /* parent process: child's exit status */

        
    /* now create new process */
    childpid = fork();
    
    if (childpid >= 0) /* fork succeeded */
    {
        if (childpid == 0) /* fork() returns 0 to the child process */
        {
            printf("CHILD: I am the child process!\n");
            printf("CHILD: Here's my PID: %d\n", getpid());
            printf("CHILD: My parent's PID is: %d\n", getppid());
            printf("CHILD: The return value of of fork for me was: %d\n", childpid);
            printf("CHILD: Sleeping for 1 second...\n");
            sleep(1); /* sleep for 1 second */
          
            retval=7; //the exit value (0 to 255)
            printf("CHILD: Goodbye!\n");    
            exit(retval); /* child exits with user-provided return code */
        }
        else /* fork() returns new pid to the parent process */
        {
            printf("PARENT: I am the parent process!\n");
            printf("PARENT: Here's my PID: %d\n", getpid());
 	    printf("PARENT: My parent's PID is: %d\n", getppid());
            printf("PARENT: The return value of of fork for me was %d\n", childpid);
            printf("PARENT: I will now wait for my child to exit.\n");
            // wait(&status); /* wait for child to exit, and store its status */
            //printf("PARENT: Child's exit code is: %d\n", WEXITSTATUS(status));
            //sleep(3);
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
