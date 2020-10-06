#include "q1.h"
 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <fcntl.h>
#include <string.h>
//#include <unistd.h>     /* Symbolic Constants */
//#include <sys/types.h>  /* Primitive System Data Types */
//#include <sys/wait.h>   /* Wait for Process Termination */
//#include <termios.h>

#define BUFSIZE 32
/* for question 1/2, use O_RDONLY */
/* for question 1/3, use O_RDWR */
#define RW_PERM /*O_RDONLY */ O_RDWR

/* read /home/mrv/mrv with RO permissions */
int func1()
{
	int fd;
	char buf[BUFSIZE] = {0};
	int read_len=-1;
	int length=0;
	
	printf("func1() read with RO permissions:\n");
	printf("=================================\n");

	if ( (fd = open("/home/mrv/mrv", O_RDONLY | O_CREAT, 0666)) < 0 )
	{
		perror("failed to open /home/mrv/mrv");
		exit(errno);
	}
	
	/* Read up to max size of buf. */
	length=BUFSIZE;
	read_len = read (fd, buf, length);
	if (read_len < 0) 
	{
		/* reading file failed. Print an error message and exit. */
		perror("error reading file");
		exit(-2);
	}
    printf("%d bytes have been read from file \n",read_len);
	/* make sure buf is ended with '\0' */
	if (read_len == BUFSIZE) buf[BUFSIZE-1] = '\0';
	printf("file content = %s", buf);
	
	close(fd);
	return 0;
}

/* trying to write with O_RDONLY permissions
   trying to write with O_RDONLY permissions but with another file descriptor 
   trying to write with O_RDWR permissions */
int func2()
{
	int fd /*,fd1*/;
	char buf[BUFSIZE] = "1234567890";
	int write_len=-1;
	int read_len=-1;
	int length=0;

	printf("func2() write with RO permissions:\n");
	printf("==================================\n");

	if ( (fd = open("/home/mrv/q.txt", RW_PERM | O_CREAT, 0666)) < 0 )
	{
		perror("failed to open /home/mrv/mrv");
		exit(errno);
	}

	/* Read up to max size of buf. */
	write_len =  write(fd, buf, strlen(buf));
	if (write_len < 0) 
	{
		/*  writing to file failed. Print an error message and exit. */
		perror("error writing file");
		exit(errno);
	}
    printf("%d bytes have been written to file \n",write_len);
	
	/* !!!!!! check by read !!!!!!!!!!!!! */
	/* Rewind to the beginning of the file. */
	lseek (fd, 0, SEEK_SET);

	/* Read up to max size of buf. */
	length=strlen(buf);
	read_len = read (fd, buf, length);
	if (read_len < 0) 
	{
		/* reading file failed. Print an error message and exit. */
		perror("error reading file");
		exit(errno);
	}
    printf("%d bytes have been read from file \n",read_len);
	/* make sure buf is ended with '\0' */
	if (read_len == BUFSIZE) buf[BUFSIZE-1] = '\0';
	printf("file content = %s", buf);
	printf("\n");
	
	close(fd);
	return 0;
}

/* cp /home/mrv/mrv to a new file a.txt */
int func3(char *path)
{
	int fd, fd1;
	char *buf;
	int read_len=-1;
	int write_len=-1;
	int length=0;

	if (!path)
	{
		printf("file path is NULL\n\r");
		return -1;
	}
	
	printf("func3() cp a file %s to ~/a.txt:\n", path);
	printf("===========================================\n");

	if ( (fd = open(path, O_RDONLY | O_CREAT, 0666)) < 0 )
	{
		perror("failed to open user file");
		exit(errno);
	}

	/*Upon successful completion, lseek() returns the resulting offset location as measured in bytes from the beginning of the file.
	  On error, the value (off_t) -1 is returned and errno is set to indicate the error. */
	if ((length = lseek(fd, 0, SEEK_END)) < 0 )
	{
		perror("failed to lseek user file");
		exit(errno);
	}
	printf("lseek returned the file length in bytes: %d\n",length);

	/* Allocate a buffer and read the data. */
	buf = (char*) malloc (length);
	if (buf == NULL)
	{
		perror("error allocating Memory");
		exit(errno);
	}
	printf("Allocated buffer address is  %p \n",buf); 

	/* Read up to max size of buf. */
	/* Rewind to the beginning of the file. */
	lseek (fd, 0, SEEK_SET);
	read_len = read (fd, buf, length);
	if (read_len < 0) 
	{
		/* reading file failed. Print an error message and exit. */
		perror("error reading file");
		exit(errno);
	}
    printf("%d bytes have been read from file \n",read_len);
	
	if ( (fd1 = open("/home/mrv/a.txt", O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0 )
	{
		perror("failed to open /home/mrv/a.txt");
		exit(errno);
	}
	
	write_len = write (fd1, buf, length);
	if (write_len < 0) 
	{
		/* writing the file failed. Print an error message and exit. */
		perror("error writing file");
		exit(errno);
	}
	printf("%d bytes have been written to file \n",write_len); 
	
	free(buf);
	close(fd);
	close(fd1);

	return 0;
}

int func4()
{
	int fd;
	char buf[BUFSIZE] = {0};
	int read_len=-1;
	int length=0;
	
	printf("func4() reads /dev/zero:\n");
	printf("=================================\n");

	if ( (fd = open("/dev/zero", O_RDONLY, 0666)) < 0 )
	{
		perror("failed to open /dev/zero");
		exit(errno);
	}
	
	/* Read up to max size of buf. */
	length=BUFSIZE;
	read_len = read (fd, buf, length);
	if (read_len < 0) 
	{
		/* reading file failed. Print an error message and exit. */
		perror("error reading file");
		exit(-2);
	}
    printf("%d bytes have been read from /dev/zero \n",read_len);
	/* make sure buf is ended with '\0' */
	if (read_len == BUFSIZE) buf[BUFSIZE-1] = '\0';
	printf("file content = %s", buf);
	int i;
	for (i = 0; i < read_len; i++)
	{
		printf("char in buf[%d] = %d\n\r", i, buf[i]);
	}
	
	close(fd);
	return 0;
}

int func5()
{
	int fd;
	char buf[BUFSIZE] = {0};
	int write_len =-1;
	
	printf("func5() is trying to write /dev/full:\n");
	printf("========================================\n");

	if ( (fd = open("/dev/full", O_RDWR, 0666)) < 0 )
	{
		perror("failed to open /dev/full");
		exit(errno);
	}
	
	write_len = write (fd, buf, BUFSIZE);
	if (write_len < 0) 
	{
		/* writing the file failed. Print an error message and exit. */
		perror("error writing file");
		exit(errno);
	}
	printf("%d bytes have been written to file /dev/full \n",write_len);	
	
	close(fd);
	return 0;
}

extern char **environ;
int func6()
{
	int fd;
	int write_len =-1;
    char **var;
	char *file = strcat(getenv("HOME"), "/environment");
	
	printf("func6() is writing all ENV variables into %s:\n", file);
	printf("====================================================\n");

	if ( (fd = open(file/*"/home/$USER/envrionment"*/, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0 )
	{
		perror("failed to open /home/mrv/envrionment");
		exit(errno);
	}
	
   for(var=environ; *var!=NULL; ++var)
   {
        //printf("%s\n",*var);
		write_len = write (fd, *var, strlen(*var));
		if (write_len < 0) 
		{
			/* writing the file failed. Print an error message and exit. */
			perror("error writing file");
			exit(errno);
		}
		write(fd, "\n", 1);
	}


   return 0;
}
