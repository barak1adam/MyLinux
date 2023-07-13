/* valgrind_invalid_free.c
 * classical leak: one memory allocation, no free...
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#define NUM_INTS 4

int main (void)
{
	int i;

	/* allocate some memory on the heap */
	int *block = malloc(NUM_INTS * sizeof(int));

	/* put some values in the block [0..NUM_INT-1] */
	for (i = 0; i < NUM_INTS; i++)
	{
		block[i] = i;
	}

	//no free...


	return 0;
}

/********** valgrind output ****************************************************
 *
==1019== HEAP SUMMARY:
==1019==     in use at exit: 16 bytes in 1 blocks
==1019==   total heap usage: 1 allocs, 0 frees, 16 bytes allocated
==1019==
==1019== LEAK SUMMARY:
==1019==    definitely lost: 16 bytes in 1 blocks
==1019==    indirectly lost: 0 bytes in 0 blocks
==1019==      possibly lost: 0 bytes in 0 blocks
==1019==    still reachable: 0 bytes in 0 blocks
==1019==         suppressed: 0 bytes in 0 blocks
==1019== Rerun with --leak-check=full to see details of leaked memory
**
* ****************************************************************************/

