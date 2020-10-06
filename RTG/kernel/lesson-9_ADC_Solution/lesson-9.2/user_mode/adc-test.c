#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h>
#include <string.h>

int main(void)
{
	fprintf(stderr, "wait 10 seconds or press Ctrl-C to stop\n");
	int fd = open("/dev/ADC_onchip_A2D", 0);
	int i;
	if (fd < 0) {
		perror("open ADC device:");
		return -1;
	}

	for(i=0;i<10;i++)
	{
		char buffer[30];
		int len = read(fd, buffer, sizeof(buffer-1) );
		if (len > 0)
		{
			buffer[len] = '\0';
			int value = -1;
			sscanf(buffer, "%d", &value);
			printf("ADC Value: %d\n", value);
		} 
		else 
		{
			perror("read ADC device:");
			return 1;
		}
		usleep(1000* 1000);
	}
	
//	close(fd);
	return 0;
}
