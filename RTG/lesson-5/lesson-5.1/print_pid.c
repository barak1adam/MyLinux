//Listing 5.1 ( print-pid.c) Printing the Process ID


#include <stdio.h>
#include <unistd.h>


/* Questions
1. enable the for loop starting on line 15 , open another terminal and check
   if the pid exists
2. kill the other terminal
*/

int main ()
{
	int counter=0;
	printf ("The process ID is %d\n", (int) getpid ());
	printf ("The parent process ID is %d\n", (int) getppid ());
	
	for(;;)
	{
		sleep(2);
                printf ("counter = %d\n", counter++);
	}
	return 0;
}

