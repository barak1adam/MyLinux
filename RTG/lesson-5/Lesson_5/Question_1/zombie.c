/*
Example code for creating a zombie process:
1. Creates child process
2. Prints pid and ppid in each process
3. let the child exit and keep the parent alive with sleep(). parent is not calling wait() which is leaving the child as a zombie.

Explanation:
============
A zombie process is a process that has completed execution but still has an entry in the process table.
This occurs for child processes, where the process is ended but was not "waited" by its parent.
When a process ends via exit, all of the memory and resources associated with it are deallocated so they can be used by other processes.
However, the process's entry in the process table remains. 
In addition when a process terminates, the kernel sends the signal SIGCHLD to the parent. 
By default, this signal is ignored, and no action is taken by the parent.
If parent is alive and not waiting for its child's SIGCHILD, the child is zombie.

What happens when the parents dies?
-----------------------------------
When the parent program exits, the child process is inherited by init.
Inits waits on every child so it should clean up the zombie proces automatically.

How to see the zombie processes?
--------------------------------
Run the following command in a new bash window: ps -e -o pid,ppid,stat,cmd
The child process is marked as <defunct>, and its status code is Z, for zombie.

What is the problem with zombies?
---------------------------------
Since there is no memory allocated to zombie processes – the only system memory usage is for the process table entry itself
The primary concern with many zombies is not running out of memory, but rather running out of process table entries, concretely process ID numbers.

How to avoid zombies?
---------------------
The parent can read the child's exit status by executing the wait system call
once the exit status is read via the wait system call, the zombie's entry is removed from the process table and it is said to be "reaped".


4. How to pass information from parent process to child process?
Answer: We can use IPC for sending information between parent-child processe.
*/


#include <stdio.h>      // IO operations
#include <unistd.h>   	//for pid, ppid, fork functions
#include <sys/types.h>  //for pid_t type, kill function
#include <stdlib.h> 	//for exit function
#include <signal.h>	//for kill function

int main ()
{
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
		sleep(60); /* child is a zombie now. As long as the parent is alive and was not waiting for the child, the child is a zombie. */ 
	}
	return 0;
	/* no zombie now: parent is dieng. child is automatically reaped by its new parent - the init process. */
}
