//Listing 5.2 (system.c) Using the system Call
#include <stdlib.h>
#include <stdio.h>      /* Input/Output */

int main ()
{
	int return_value;
	return_value = system ("ls -l /");
	printf("return value after calling systems call is %d\n",return_value);
	return return_value;
}

