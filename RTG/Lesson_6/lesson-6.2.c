/* Home work solution 6.2

Description
-----------


Note: when compiling and link this program be sure to add the -lpthread library:
as the following:

% gcc lesson-6_2.c -lpthread -ggdb -o lesson-6.2

food for thought
* --------------*
1. how can you make sure write_thread runs before read_thread
2. within the read_thread, how do you know what is the length of the name or family-name   
3. within the read_thread, you need to start reading from the begining of the file  
4. should we worry about race-conditions
/*************  includes     *****************/


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <string.h>

#include <pthread.h>
#include <stdio.h>

/*************  Defines     *****************/
#define FILEPATH "/tmp/phone.txt"


/*************  Prototypes   *****************/
void* write_func();
void* read_func();

/********s* ****  Structures   *****************/
struct entry
{
	char name[80];       // name
	char family[80];     // family-name
	char phone[80];      // phone-number
	int  entry_count;    //  number of threads created
};

/*************  Globals   *****************/
struct entry my_entry;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
int fd;
int write_flag;
/*************  main() function ****************/ 
// The main program. 
int main ()
{
	int i=0;
	pthread_t write_thread,read_thread;
	my_entry.entry_count = 0;
	write_flag=0;
	
	fd=open(FILEPATH, O_RDWR |O_CREAT,0666);	
	if (fd == -1) {
		perror("Error opening file for writing");
		exit(EXIT_FAILURE);
    }
	
	//creating threads
	pthread_create (&write_thread, NULL, &write_func, NULL);
	pthread_create (&read_thread, NULL, &read_func, NULL);
	
	
	
	pthread_join (write_thread, (void*) NULL);
  	pthread_join (read_thread, (void*) NULL);

	return 0;
}

/*************  write_func() function ****************/
void* write_func ()
{
	int i=0;
	pthread_mutex_lock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	scanf("%s",my_entry.name);
	scanf("%s",my_entry.family);
	scanf("%s",my_entry.phone);
	my_entry.entry_count++;
	
	printf("write_func() %s %s %s %d ",
		my_entry.name,
		my_entry.family,
	    my_entry.phone,
	    my_entry.entry_count);
	    
	write(fd,my_entry.name,strlen(my_entry.name) );
	write(fd,my_entry.family,strlen(my_entry.family) );
	write(fd,my_entry.phone,strlen(my_entry.phone) );
	write(fd,&my_entry.entry_count ,sizeof(my_entry.entry_count) );
	
	write_flag =1;
	
	pthread_mutex_unlock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	return;
}

/*************  write_func() function ****************/
void* read_func ()
{
	struct entry the_entry;
	int result;
	int flag=0;
	
	while(!flag)
	{ 
		pthread_mutex_lock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		flag = write_flag;
		pthread_mutex_unlock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		printf(" read_func()  going to sleep \n ");
		sleep(1);
	}
	
	printf(" \n \n ");
	pthread_mutex_lock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	result = lseek(fd, 0, SEEK_SET);
    if (result == -1) {
		close(fd);
		perror("Error calling lseek() to 'stretch' the file");
		exit(EXIT_FAILURE);
    }
	read(fd,the_entry.name,strlen(my_entry.name) );
	read(fd,the_entry.family,strlen(my_entry.family) );
	read(fd,the_entry.phone,strlen(my_entry.phone) );
	read(fd,&the_entry.entry_count ,sizeof(my_entry.entry_count) );
	
	printf(" read_func() %s %s %s %d ",
		the_entry.name,
		the_entry.family,
	    the_entry.phone,
	    the_entry.entry_count);
	    
	pthread_mutex_unlock(&my_mutex); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	return;
}


