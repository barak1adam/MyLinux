/*
Question: what is the output of the following code?
Answer: The output is endless '1'-s. 
Explanation: When fork() is performed by the parent process, another process is created the child process.
             fork() for the parent process returns child pid, so the parent prints "1" and calls fork() again.
             fork() for the child process returns 0. so the child exits the program.
	         the problem caused is: the child processes become zombies.
*/
#include <stdio.h>      // IO operations
#include <unistd.h>   	//for pid, ppid, fork functions

int main ()
{
	pid_t childpid;

	while (childpid=fork() != 0)
	{
		printf ("1");
	}
	return 0;
}
