/*
1. Create file //tmp//a.txt
2. Create two processes: the parent process writes to the file "Welcome to Linux Embedded"
 and the child process reads from the file and prints it to the screen.

Questions: 
1) What is the main problem with the program? - The main problem is that there are no common variables between the processes
2) How you can synchronize the processes? - The way to synchronize is sending signals.
*/
#include <stdio.h>      // IO operations
#include <unistd.h>   	//for pid, ppid, fork functions
#include <sys/types.h>  //for pid_t type, kill function
#include <stdlib.h> 	//for exit function
#include <signal.h>	//for kill function
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>   //for wait
//Globals
int fd = -1;
sig_atomic_t sigusr1_count = 0;
//Prototypes
void signal_handler (int signal_number);
/**********************************************************************************/
int main()
{
	pid_t childpid;
	char string[80] = "Welcome to Linux Embedded";
	struct sigaction sa;
	char buffer[80] = {0};	
	int length;
	int status;


	childpid = fork();

	if (childpid < 0)
	{
		perror ("fork failed");
		exit (1);
	}
	else
	{
		if (childpid == 0)
		{
			//child process
			memset (&sa, 0, sizeof (sa));
			sa.sa_handler = &signal_handler;
			sigaction (SIGUSR1, &sa, NULL);
			while(!sigusr1_count)
			{
				sleep(1);
			}
		}
		else
		{
			//parent process
			fd = open ("//tmp//a.txt", O_WRONLY | O_CREAT,0666);
			if (fd == -1) 
			{
				/* The open failed. Print an error message and exit. */
				printf("error opening file\n");
				exit(-1);
			}

			length = write(fd, string, strlen(string));
			close(fd);
			if (-1 != length)
			{
				printf("PARENT: write to file: %s\n", string);
				kill(childpid, SIGUSR1); //send a signal to the child process
			}
			wait(&status); //wait for the child process to finish
			printf("PARENT: the child process finished\n");
		}
	}

	close(fd);
	return 0;
}
/**********************************************************************************/
void signal_handler (int signal_number)
{
	char buffer[80] = {0};
	int length;

	
	printf("CHILD: signal recieved: %d\n", signal_number);

	fd = open ("//tmp//a.txt", O_RDONLY | O_CREAT,0666);
	if (fd != -1)
	{
		length = read(fd, buffer, sizeof(buffer));
		close(fd);
		if (-1 != length)
		{
			printf("CHILD: read from file: %s\n", buffer);
		}
		
	}
	++sigusr1_count;
}
