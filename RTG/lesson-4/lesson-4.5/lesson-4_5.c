// Listing 4.6 usage of UART using Linux O.S.

/*************  includes     *****************/

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <termios.h>
#include <fcntl.h>
#include <string.h>


#define SERIAL_DEVFILE_1 "/tmp/ttySAC1"
#define SERIAL_DEVFILE_2 "/tmp/ttySAC2"

/*************  Prototypes   *****************/

void config_serial(int fd_serial);



/*************  main() function ****************/ 
int main()
{
	int ret, fd_serial1,fd_serial2;
    struct termios options;
    int n;
    char tx_buff[255];
    char rx_buff[255];
	
	// initialize serial connection
	printf(" initialize serial file descriptors\n");
	
    /* Open Ports */
	fd_serial1 = open(SERIAL_DEVFILE_1, O_RDWR | O_NOCTTY | O_NDELAY);  /* <--- serial port 1 */
	if(fd_serial1 == -1) {
		printf("ERROR Open Serial Port 1!");
		exit(-1);
    }
    
	fd_serial2 = open(SERIAL_DEVFILE_2, O_RDWR | O_NOCTTY | O_NDELAY);  /* <--- serial port 2 */
	if(fd_serial1 == -1) {
		printf("ERROR Open Serial Port 2!");
		exit(-1);
    }
    
    // Serial Configuration 
	config_serial(fd_serial1); 
	config_serial(fd_serial2); 
	
	//writing tx-buffer to serial port
	strcpy(tx_buff,"YOUR COMMAND STRING HERE \n");
    ret = write(fd_serial1,tx_buff,strlen(tx_buff) );
    if (ret == -1) {
		perror("Error writing to device");
		exit(EXIT_FAILURE);
    }
    
    sleep(1); //wait for HW to write to device
    
    //reading serial port into rx-buffer
    ret = read(fd_serial2,rx_buff,strlen(tx_buff) );
    if (ret == -1) {
		perror("Error writing to device");
		exit(EXIT_FAILURE);
    }
    
  close(fd_serial1); // Close Port
  close(fd_serial2); // Close Port
  return 0; // End Program
}

/////////////////////////////////////////////////////////
void config_serial(int fd_serial)
{
	struct termios options;

  	tcgetattr(fd_serial, &options);   // Get Current Config
  	cfsetispeed(&options, B9600);     // Set Baud Rate
	cfsetospeed(&options, B9600);
	options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;
        options.c_iflag =  IGNBRK;
        options.c_lflag = 0;
        options.c_oflag = 0;
        options.c_cflag |= CLOCAL | CREAD;
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 5;
        options.c_iflag &= ~(IXON|IXOFF|IXANY);
        options.c_cflag &= ~(PARENB | PARODD);
  
        /* Save The Configure */
        tcsetattr(fd_serial, TCSANOW, &options);
        /* Flush the input (read) buffer */
        tcflush(fd_serial,TCIOFLUSH);
}
/////////////////////////////////////////////////////////
