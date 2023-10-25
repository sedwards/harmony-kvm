#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
/* baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h> */
#define BAUDRATE B9600            
/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


int main(int argc, char *argv[])
{
	volatile int STOP=FALSE;
       
	int res,fd;
	struct termios oldtio,newtio;
	char buf[255];
    char *msg="hello world\n";
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
	if (fd <0) {perror(MODEMDEVICE); return(-1); }
        
	tcgetattr(fd,&oldtio); /* save current port settings */
        
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
        
	/* set input mode */
	newtio.c_lflag = 0;
         
	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 char received */
        
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);
		
	char tmptmp[20];
    int tmpInt;    
	while (STOP==FALSE) 
	{       /* loop for input */
		res = read(fd,tmptmp,20);   // returns after up to 20 chars has been received 
		printf("received: %s",tmptmp);	
	}
	/* you can write using write system call like so: write(fd, dataToWrite, length) */
	tcsetattr(fd,TCSANOW,&oldtio);
}

