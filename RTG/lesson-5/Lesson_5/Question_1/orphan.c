/*
Code making the following
1. Creates child process
2. Prints pid and ppid in each process
3. Add some sleep() to the parent process, and some long (or endless) loop to the child process. What happens?
Answer: A child running with no parent is called an orphan process. 
These orphan processes do not remain as zombie processes; instead, they are adopted by init (process ID 1), which waits on its children.
The result is that an orphan process that is ended, will not become a zombie but will be reaped automatically.

So will see a child process running with a ppid being changed from parent pid to init pid (1) upon parent termination (after parent finish sleeping and terminates).
Later when the child is terminated too, it will be automatically removed by its new parent - the init process.
So we will have no zombie childs left as a result.

Benny - This answer is not correct:
===================================
If there is an endless loop in the child process and the parent process didn't call wait,
then the parent process terminates before the child process and this is BAD, 
since there isn't a process to clean up after the child process.

4. How to pass information from parent process to child process?
Answer: We can use IPC for sending information between parent-child processe.
*/

#include <stdio.h>      // IO operations
#include <unistd.h>   	//for pid, ppid, fork functions
#include <sys/types.h>  //for pid_t type, kill function
#include <stdlib.h> 	//for exit function
#include <signal.h>	//for kill function - I would suggest students to use wait() instead

int main ()
{
	pid_t pid = fork();
	
	if (0 == pid) //child process
	{
		int i;
		for (i = 0; i<20; i++){
			printf ("I'm the child process: pid=%d, ppid=%d\n", getpid(), getppid());
			sleep(1);
		}
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
		sleep(10); /* here both the parent and the child are alive. no zombies, no orphans */
	}
	/* here the parent is dieng, but the child is still running in a long loop and become an orphan. its PPID is changed to 1 */
	return 0;
}
