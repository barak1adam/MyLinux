/* valgrind_invalid_read_write.c
 * This is a simple example of a program which makes invalid reads & invalid writes of memory.
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

	/* allocate some memory for a string on the heap */
	char *str = malloc(strlen("mrv string"));

	/* a classical strlen bug - strlen returns with 10. need to allocate strlen + 1, e.g. 11 */

	/* copy 11 bytes into a block of 10 ... */
	/* this is an invalid write ! */
	strcpy(str, "mrv string");

	/* printf looks fine, if we lucky to have an ended 0 */
	/* reading the 11 char is an invalid read, out of bounds */
	printf("str = %s\n\r", str);

	/* free the allocated memory */
	free(str);

	return 0;
}

/********** valgrind output ****************************************************
 *
==1012== Invalid write of size 1
==1012==    at 0x84FC: main (valgrind_invalid_read_write.c:25)
==1012==  Address 0x499a032 is 0 bytes after a block of size 10 alloc'd
==1012==    at 0x4838328: malloc (vg_replace_malloc.c:299)
==1012==    by 0x84CF: main (valgrind_invalid_read_write.c:19)
==1012==
==1012== Invalid read of size 1
==1012==    at 0x4838DE8: strlen (vg_replace_strmem.c:454)
==1012==    by 0x48A2FBB: vfprintf (in /lib/libc.so)
==1012==  Address 0x499a032 is 0 bytes after a block of size 10 alloc'd
==1012==    at 0x4838328: malloc (vg_replace_malloc.c:299)
==1012==    by 0x84CF: main (valgrind_invalid_read_write.c:19)
*
* ****************************************************************************/
