#include <termios.h>


myFD = open("/dev/ttyTS0", O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (myFD<0){return 1;} 

   	bzero(&TermIO, sizeof(TermIO));
	TermIO.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	TermIO.c_lflag = ICANON;
	TermIO.c_iflag = IGNPAR | IGNCR | IGNBRK;
	TermIO.c_oflag = 0;
	TermIO.c_cc[VINTR]    = 0;     
	TermIO.c_cc[VQUIT]    = 0;     
	TermIO.c_cc[VERASE]   = 0;    
	TermIO.c_cc[VKILL]    = 0;     
	TermIO.c_cc[VEOF]     = 0;
	TermIO.c_cc[VTIME]    = 0;     
	TermIO.c_cc[VMIN]     = 1;     
	TermIO.c_cc[VSWTC]    = 0;     
	TermIO.c_cc[VSTART]   = 0;    
	TermIO.c_cc[VSTOP]    = 0;     
	TermIO.c_cc[VSUSP]    = 0;     
	TermIO.c_cc[VEOL]     = 0;    
	TermIO.c_cc[VREPRINT] = 0;    
	TermIO.c_cc[VDISCARD] = 0;    
	TermIO.c_cc[VWERASE]  = 0;    
	TermIO.c_cc[VLNEXT]   = 0;     
	TermIO.c_cc[VEOL2]    = 0;   
	tcflush(FDGPS, TCIFLUSH);
	tcsetattr(FDGPS,TCSANOW,&TermIO);

// Examples only

LenRead = read(myFD,Buf,LenBuf) ;

write(myFD,outBuf,outLen);

