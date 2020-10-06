// Listing 5.8 ( fork-wait2.c) Using macros to fins out Process exit code.

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
	int status;
	pid_t pid;
	if (!fork ( ))
		return 1;
        
    pid = wait (&status);
	
	if (pid == -1)
		perror ("wait");
	
	printf ("pid=%d\n", pid);
	
	if (WIFEXITED (status))
		printf ("Normal termination with exit status=%d\n",
				 WEXITSTATUS (status));
				 
	if (WIFSIGNALED (status))
		printf ("Killed by signal=%d%s\n",
				WTERMSIG (status),
				WCOREDUMP (status) ? " (dumped core)" : "");

	if (WIFSTOPPED (status))
		printf ("Stopped by signal=%d\n",
		    	WSTOPSIG (status));

	if (WIFCONTINUED (status))
		printf ("Continued\n");
	
	return 0;
}
