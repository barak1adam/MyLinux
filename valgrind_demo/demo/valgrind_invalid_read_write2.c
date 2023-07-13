/* valgrind_invalid_read_write2.c
 * This is a simple example of a program which makes invalid reads and invalid writes of memory.
 * memory override:
 * ----------------
 * It shows how str1 overrides str2. str2 prints OK eventhough no string was assigned to it (it is under a comment...)
 *
 * if we try to free str1, valgrind will also complain about an invalid free, because str1 block is not ended where it is expected.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main (void)
{
	/* NOTE: "mrv string" length is total 11:  10 chars + '\0' char */

	/* allocate the correct amount of memory for a string on the heap */
	char *str1 = malloc(strlen("mrv string") + 1);
	char *str2 = malloc(strlen("mrv string") + 1);

	/* copy a much bigger string into str1 */
	/* this is an invalid write ! */
	strcpy(str1, "mrv string mrv string");

	/* a correct copy into str2 */
	//strcpy(str2, "mrv string");

	/* printf of str1 looks fine, even we had an invalid write */
	printf("str1 = %s\n\r", str1);

	/* printf of str2 */
	printf("str2 = %s\n\r", str2);

	/* free the allocated memory */
	//free(str1);
	//free(str2);

	return 0;
}
