//Listing 5.5 (signal.c) Using a Signal signal_handler

//includes
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

//Globals
sig_atomic_t sigusr1_count = 0;

//Prototypes
void signal_handler (int signal_number);
/**********************************************************************************/
int main ()
{
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &signal_handler;
	sigaction (SIGUSR1, &sa, NULL);
	
	/* Do some lengthy stuff here. */
	/* ... */
	for (;;)
	{
		sleep(1);
		printf ("SIGUSR1 was raised %d times\n", sigusr1_count);
	}	
	return 0;
}
/**********************************************************************************/
void signal_handler (int signal_number)
{
	++sigusr1_count;
}


