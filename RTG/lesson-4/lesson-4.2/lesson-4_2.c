/*
 * This program illustrates usage of a temporary file ceated by the OS
 * 1. At first glance the progrm doesn't work, Why ?
 * 2. What is the unlink call used for ?
 * 3. What is the program used for ? 
 * 4. Do you recognize an potential bugs ?
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
 * defines
 */
/* A handle for a temporary file created with write_temp_file. In
	this implementation, its just a file descriptor. */
typedef int temp_file_handle;


/*------------------------------------------------------------------
 * prototypes
 */
temp_file_handle write_temp_file (char* buffer, size_t length);
char* read_temp_file (temp_file_handle temp_file);


/*******************************************************************************/
int main()
{

	char buffer[]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; 
	char *pHeap=NULL; 
	size_t length=strlen(buffer);
	temp_file_handle fd=-1;

	fd=write_temp_file(buffer,length);
	pHeap = read_temp_file (fd);
	
	/* Close the file descriptor, which will cause the temporary file to
		go away. */
	close (fd);
}
/*******************************************************************************/	
/* 
 * Writes LENGTH bytes from BUFFER into a temporary file. 
 * The tmporary file is immediately unlinked. 
 * Returns a handle to the temporary file.
 */
temp_file_handle write_temp_file (char* buffer, size_t length)
{
		
/* Create the filename and file. 
 * The XXXXXX will be replaced with characters that make the filename unique.
 */
	char temp_filename[] = "/tmp/dir/temp_file.XXXXXX";
	int write_len=-1;
	int fd = mkstemp (temp_filename);
	if (fd == -1) 
	{
		/* The open failed. Print an error message and exit. */
		perror("error opening file ");
    	exit(-1);
	}
	printf("opening file succedded fd=%d \n",fd); 
				
	/* Write the number of bytes to the file first. */
	write_len = write (fd, &length, sizeof (length));
	if (write_len < 0) 
	{
		/* writing the file failed. Print an error message and exit. */
		perror("error writing file");
		exit(-5);
	}
	printf("buffer length = %lu has been written to file \n",length);
	
	/* Now write the data itself. */
	write_len = write (fd, buffer, length);
	if (write_len < 0) 
	{
		/* writing the file failed. Print an error message and exit. */
		perror("error writing file");
		exit(-6);
	}
	printf(" %d data bytes have been written to temp file \n",write_len);
	
	
	/* Unlink the file immediately, 
	 * so that it will be removed when the file descriptor is closed.
	 */
	//unlink (temp_filename);

	/* Use the file descriptor as the handle for the temporary file. */
	return fd;
}
/*******************************************************************************/
/* Reads the contents of a temporary file TEMP_FILE created with write_temp_file.
 * The return value is a newly allocated buffer of those contents, 
 * which the caller must deallocate with free.
 * 
 * LENGTH is set to the size of the contents, in bytes. 
 * The 	temporary file is removed.
 */
char* read_temp_file (temp_file_handle temp_file)
{
	char* buffer;
	size_t itsLength=0;
	int read_len=-1;
	
	/* The TEMP_FILE handle is a file descriptor to the temporary file. */
	int fd = temp_file;
	
	/* Rewind to the beginning of the file. */
	lseek (fd, 0, SEEK_SET);
	
	/* Read the size of the data in the temporary file. */
	read_len = read (fd, &itsLength, sizeof (itsLength));
	if (read_len < 0) 
	{
		/* reading file failed. Print an error message and exit. */
		perror("error reading file");
		exit(-2);
	}
	printf("the size of the data in the temporary file is %lu bytes\n",itsLength); 

	/* Allocate a buffer and read the data. */
	buffer = (char*) malloc (itsLength);
	if (buffer == NULL)
	{
		perror("error allocating Memory");
		exit(-3);
	}
	printf("Allocated buffer address is  %p \n",buffer); 

	
	read_len = read (fd, buffer, itsLength);
	if (read_len < 0) 
	{
		/* reading file failed. Print an error message and exit. */
		perror("error reading file");
		exit(-4);
	}
	printf("the amount of data read from temporary file is %d byes\n",read_len); 

	
	return buffer;
}
/*******************************************************************************/
