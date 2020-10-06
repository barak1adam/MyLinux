//Listing 5.4 ( fork-exec.c) Using fork and exec Together

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// Prototypes
int spawn (char* program, char** arg_list);

/* Questions
1.  Howcome the child's print is sometimes before the parents ???
2.  why dont we see the line number 33 "printf ("main() ...) twice ??? 
*/

/* Description
Spawn a child process running a new program. PROGRAM is the name
of the program to run; the path will be searched for this program.
ARG_LIST is a NULL-terminated list of character strings to be
passed as the program�s argument list. Returns the process ID of
the spawned process. */

/************************************************************************/
int main ()
{
	/* The argument list to pass to the �ls� command. */
	char* arg_list[] = {
		"ls", /* argv[0], the name of the program. */
			"-l",
			"/",
			NULL /* The argument list must end with a NULL. */
	};
	/* Spawn a child process running the "ls" command. Ignore the
	returned child process ID. */
	int stat = spawn ("ls", arg_list);
	printf ("main() stat=%d done with main program\n",stat);
	return 0;
}
/**************************************************************************/
int spawn (char* program, char** arg_list)
{
	pid_t child_pid;
	/* Duplicate this process. */
	child_pid = fork ();
	if (child_pid != 0)
	{
		/* This is the parent process. */
		printf ("pid = %d this is the parent process\n",child_pid);
		return child_pid;
	}
	else 
        {
		
		/* This is the child process. */
		printf ("pid = %d this is the child process\n",child_pid);
		
		/* Now execute PROGRAM, searching for it in the path. */
		execvp (program, arg_list);
		
		/* The execvp function returns only if an error occurs. */
		fprintf (stderr, "an error occurred in execvp\n");
		
		abort ();
	}
}


