/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */


/*
 * to create large files I used: dd if=/dev/urandom of=100MB.bin bs=64K count=1600
 * then I cp the large files to NFS folder
 * then I ran the program on target to read the files from the NFS
 * I used multiple instances of the program with multiple files, to increase Linux Page Cache
 * But I can also use one instance with one large file...
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <error.h>

#define MAX_LINE_NUM 64
#define MAX_LINE_LEN 32
#define BUFSIZE 32

int main (int argc, char *argv[])
{
#if 0
	FILE* fid;

	while(1){
	/* open the file */
	if ((fid = fopen("/file", "r")) == NULL)
	{
		printf("file was not opened\n\r");
		exit(2);
	}

	/* print input file line by line and copy it to input array */
	printf("Input file content:\n\r");
	printf("===================\n\r");

	char temp[MAX_LINE_NUM] = {0};
	while(fgets(temp, MAX_LINE_NUM, fid) != NULL) {
	}

	fclose(fid);
	}
#endif	

	int fd;
	char buf[BUFSIZE] = {0};
	int read_len=-1;
	int length=0;
	
	if ( (fd = open(argv[1], O_RDONLY, 0666)) < 0 )
	{
		perror("failed to open /file");
		exit(-1);
	}
	
	/* Read up to max size of buf. */
	length=BUFSIZE;
	while (1)
	{
		read_len = read (fd, buf, length);
		if (read_len < 0) 
		{
			/* reading file failed. Print an error message and exit. */
			perror("error reading file");
			exit(-2);
		}
		read_len = 0;
	}
	
	close(fd);
	return 0;
}



