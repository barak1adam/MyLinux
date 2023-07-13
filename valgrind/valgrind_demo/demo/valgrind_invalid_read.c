/* valgrind_invalid_read.c
 * This is a simple example of a program which makes invalid reads of memory.
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

	/* invalid read 1 */
	printf("The %d int value is %d\n\r", NUM_INTS, block[NUM_INTS]);

	/* invalid read 2 */
	printf("The -1st int value is %d\n\r", block[-1]);

	/* free the allocated memory */
	free(block);

	return 0;
}

/********** valgrind output ****************************************************
 *
==1010== Invalid read of size 4
==1010==    at 0x8520: main (valgrind_invalid_read.c:28)
==1010==  Address 0x499a038 is 0 bytes after a block of size 16 alloc'd
==1010==    at 0x4838328: malloc (vg_replace_malloc.c:299)
==1010==    by 0x84CF: main (valgrind_invalid_read.c:19)
==1010==
The 4 int value is 0
==1010== Invalid read of size 4
==1010==    at 0x8540: main (valgrind_invalid_read.c:31)
==1010==  Address 0x499a024 is 4 bytes before a block of size 16 alloc'd
==1010==    at 0x4838328: malloc (vg_replace_malloc.c:299)
==1010==    by 0x84CF: main (valgrind_invalid_read.c:19)
==1010==
The -1st int value is 0
==1010==
*
* ****************************************************************************/
