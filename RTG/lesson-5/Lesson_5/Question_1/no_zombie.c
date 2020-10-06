/*
Example code for using wait() to avoid a zombie child process:
1. Creates child process
2. Prints pid and ppid in each process
3. let the parent wait for completion of the child process using "wait" system call. This leaves no zombies in the system !

*/


#include <stdio.h>      // IO operations
#include <unistd.h>   	//for pid, ppid, fork functions
#include <sys/types.h>  //for pid_t type, kill function
#include <stdlib.h> 	//for exit function
#include <signal.h>	//for kill function
#include <sys/wait.h>   //for wait

int main ()
{
	int status;
	pid_t pid = fork();
	
	if (0 == pid) //child process
	{
		printf ("I'm the child process: pid=%d, ppid=%d\n", getpid(), getppid());
		exit(0);
	}
	else if (pid < 0)//fork failed
	{
		perror ("Fork failed");
		exit (1);
	}
	else // parent process
	{
		printf ("I'm the parent process: pid=%d, ppid=%d\n", getpid(), getppid());
		pid = wait(&status);
		if (WIFEXITED(status))
			printf("\n\t[%d]\tChild Process %d exited with status %d.\n",
				(int) getpid(), pid, WEXITSTATUS(status));
		sleep(30); /* no zombie now. child had been waited by the parent ! */ 
	}
	return 0;
}
