/* valgrind_init_junk.c
 * This is a simple example of a program which is using a junk pointer.
 *
 * variable not initialzed is handled by the compiler.
 * variable initialized but with junk data is handled by Valgrind...
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include <assert.h>

int main (void)
{
	int i;

	/* allocate an integer on the heap */
	int *ptr = malloc(sizeof(int));

	/* ptr is now uninitialized - it points to junk data */

	/* dereference ptr, to copy its junk into i variable on the stack */
	i = *ptr;

	/* assert: check that i == 0, otherwise quit */
	/* even if we use assert, we might still run successfully if the junk data is "luckily" 0 */
	assert (i == 0);

	printf("if we are here, we are lucky\n\r");

	/* free the allocated memory */
	free(ptr);

	return 0;


}

/********** valgrind output ****************************************************
 *
==1016== Conditional jump or move depends on uninitialised value(s)
==1016==    at 0x8524: main (valgrind_init_junk.c:31)
==1016==
* *
* ****************************************************************************/

