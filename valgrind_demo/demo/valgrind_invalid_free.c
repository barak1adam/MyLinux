/* valgrind_invalid_free.c
 * trying to free sub-blocks of memory instead of the original one block allocated
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

	//invalid free !!! should be: free(block);
	for (i = 0; i < NUM_INTS; i++)
	{
		free(&block[i]);
	}

	return 0;
}

/********** valgrind output ****************************************************
 *
==1017==
==1017== Invalid free() / delete / delete[] / realloc()
==1017==    at 0x4836DD8: free (vg_replace_malloc.c:530)
==1017==    by 0x8507: main (valgrind_invalid_free.c:30)
==1017==  Address 0x499a02c is 4 bytes inside a block of size 16 free'd
==1017==    at 0x4836DD8: free (vg_replace_malloc.c:530)
==1017==    by 0x8507: main (valgrind_invalid_free.c:30)
==1017==  Block was alloc'd at
==1017==    at 0x4838328: malloc (vg_replace_malloc.c:299)
==1017==    by 0x849F: main (valgrind_invalid_free.c:19)
==1017==
==1017==
==1017== HEAP SUMMARY:
==1017==     in use at exit: 0 bytes in 0 blocks
==1017==   total heap usage: 1 allocs, 4 frees, 16 bytes allocated
==1017==
*
* ****************************************************************************/

