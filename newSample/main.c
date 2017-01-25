#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "oi.h"
#include "serial.h"

enum bool {false, true};
typedef unsigned char byte;
unsigned char color = 255;

void init(Serial* serial, byte state)
{
    serialClose(serial);
    serialOpen(serial, "/dev/ttyUSB0", B115200, true);
    serialSend(serial, 128);    // Send Start
    serialSend(serial, state);    // Send state

    byte b;
    while( serialNumBytesWaiting(serial) > 0 )
   	 serialGetChar(serial, &b);
};

unsigned char bump(Serial* serial)
{
    unsigned char c = 0;
    serialSend(serial, CmdSensors);
    serialSend(serial, 7); //bumps and wheel drops
    serialGetChar(serial, &c);
    c &= 3; //example, c&=3 discards wheel drops
    return c;
};

void changeColor(Serial* serial, unsigned char c)
{
    serialSend(serial, 139);
    serialSend(serial, 4);
    serialSend(serial, c);
    serialSend(serial, 255); //was half intensity (128), now full intensity
};

void activateLED(Serial* serial, unsigned char c)
{
    serialSend(serial, 139);
    serialSend(serial, c);

    //clean/power must be resent. debris/checkRobot need 3bytes after opcode:139
    serialSend(serial, color);
    serialSend(serial, 128);
};

int main(void)
{
    unsigned char stepCount = 16;
    
    const unsigned char BOTH_BUMPERS = 3;
    const unsigned char LEFT_BUMPER = 2;
    const unsigned char RIGHT_BUMPER = 1;
    unsigned char returnSignal = 0;
    
    Serial serial;
    init(&serial, 132); //full mode
    
    // display initial color
    changeColor(&serial, color);

    while (true)
    {
	    //time_t start;
	    //start = time(NULL);
	    struct timespec tim;
	    tim.tv_nsec = 100000000; // 1 / 10 seconds
    
    /*
    time_t start = time(NULL);
    int opCount = 0;
    while ((time(NULL) - start) < 1) {
        printf("%d\n", opCount);
        opCount = opCount + 1;
        nanosleep(&tim, NULL);
    }
    */
	    int opCount = 0;
	    while (opCount < 10) {
		    //get bump sensor data
		    returnSignal = bump(&serial);
		    
		    //determine if bump occured
		    if (returnSignal != 0) {
			    if (returnSignal == BOTH_BUMPERS) {
				    activateLED(&serial, 8); // check robot led
				    activateLED(&serial, 1); // debris led
			    }
			    else {
				    if (returnSignal == LEFT_BUMPER) {
					    activateLED(&serial, 8); // check robot led
				    }
				    if (returnSignal == RIGHT_BUMPER) {
					    activateLED(&serial, 1); // debris led
				    }
			    }
		    }
		    opCount = opCount + 1;
		    nanosleep(&tim, NULL);
	    }
	    
	    if ((color - stepCount) > 0) { // this works up to stepCount - 1
		    color -= stepCount;
		    changeColor(&serial, color);
	    } else { // finishes off remaining iteration
		    if (color > 0) {
			    color = 0;
   			    changeColor(&serial, color);
   		    } else { // color = 0
   			    color = 255;
			    changeColor(&serial, color);
   		    }
	    }
    }
	return 0;
}
