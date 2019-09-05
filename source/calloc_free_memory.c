/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
	char *ptr = NULL;
	int size, total_bytes;
	

	while (1){

	printf("please enter size: ");
	scanf("%d", &size);

	if (size == 0) exit(0);

	total_bytes += size;

	size*=1024;

	printf("Program is about to allocate %d * %d of bytes in memory ! \n\r", size, sizeof(char));

	if(!(ptr=calloc(size, sizeof(char))))
	{
		fprintf(stderr,"Insufficient memory");
		exit(EXIT_FAILURE);
	}
	else
	{
		int i;
		for( i=0 ; i < size ; i++ ) {
		      ptr[i] = 0xFF;
		}

	}

	printf("MALLOC OK, total size = %d k !!!\n\r", total_bytes);

	}

	return 0;
}


