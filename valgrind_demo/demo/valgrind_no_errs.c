/* valgrind_no_errs.c
 * This is a simple example of a program which is free of Valgrind errors:
 * no memory errors, no heap leaks !
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

	free(block);

	return 0;
}


/********** valgrind output ****************************************************
 *
 * 		this should be the output:
 *
 * ==1009== HEAP SUMMARY:
 * ==1009==     in use at exit: 0 bytes in 0 blocks
 * ==1009==   total heap usage: 1 allocs, 1 frees, 16 bytes allocated
 * ==1009==
 * ==1009== All heap blocks were freed -- no leaks are possible
 * ==1009==
 * ==1009== For counts of detected and suppressed errors, rerun with: -v
 * ==1009== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
 *
 * ****************************************************************************/

