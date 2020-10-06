// Listing 45 useage oF UART using Linux O.S.




/* Includes */

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
 

/************************************************************************************/
#include <termios.h>
#include <fcntl.h>
#include <string.h>

int main()
{
  int fd;
  struct termios options;
  int n;
  char buff[255];

  /* Open Port */
//  fd = open("/dev/ttySAC1", O_RDWR | O_NOCTTY | O_NDELAY);  /* <--- YOUR PORT */
 fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);  /* <--- YOUR PORT */


  if(fd == -1) {
    printf("ERROR Open Serial Port!");
    exit(-1);
  }
  
  /* Serial Configuration */
  tcgetattr(fd, &options);   // Get Current Config
  cfsetispeed(&options, B9600); // Set Baud Rate
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
  tcsetattr(fd, TCSANOW, &options);
  /* Flush the input (read) buffer */
  tcflush(fd,TCIOFLUSH);


   write(fd,"YOUR COMMAND STRING HERE",24);

  do{
    n = read(fd,buff,255);   // Read Data From Serial Port
    buff[n] = 0;
    if(n>0)
    {
      printf("%s",buff); // Print Out
    }
  }while(strncmp(buff,"bye",3)); // If user say bye then Exit

  close(fd); // Close Port
  return 0; // End Program
}


