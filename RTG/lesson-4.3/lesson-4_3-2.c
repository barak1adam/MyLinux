
/*
 * This program illustrates usage of reading and writing to files
 * The program doesn't work, try to figure out Why
 */
 
 
/*------------------------------------------------------------------
 * includes
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>  //used for exit()
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <error.h>

/*------------------------------------------------------------------
 * prototypes
 */


/*******************************************************************************/
int main()
{
	int fd,length=0;
	char read_buffer[80];
	char write_buffer[]="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
	int read_len=-1;
	int write_len=-1;
	
	//open the file, if it doesn't exist create it with the following permission access
	fd = open ("/tmp/a.txt", O_RDWR | O_CREAT,0666);
	if (fd == -1) 
	{
		/* The open failed. Print an error message and exit. */
		perror("error opening file ");
		exit(-1);
	}
	printf("opening file succedded fd=%d \n",fd); 
	
	length = sizeof (write_buffer);

	for(;;)
	{	
		/* Read the size of the data in the file. */
		read_len = read (fd, read_buffer, length);
		if (read_len < 0) 
		{
			/* reading file failed. Print an error message and exit. */
			perror("error reading file");
			exit(-2);
		}
		printf("%d bytes have been read from file \n",read_len); 

		write_len = write (fd, write_buffer,length);
		if (write_len < 0) 
		{
			/* writing the file failed. Print an error message and exit. */
			perror("error writing file");
			exit(-3);
		}
		printf("%d bytes have been written to file \n",write_len); 
		sleep(1);
	}
		// Close the file descriptor, 
	close (fd);
         
        
}
           
