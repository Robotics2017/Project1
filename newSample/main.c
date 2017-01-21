#include <stdio.h>
#include <unistd.h>
#include "oi.h"
#include "serial.h"

enum bool {false, true};
typedef unsigned char byte;

void init(Serial* serial, byte state)
{
	serialClose(serial);
	serialOpen(serial, "/dev/ttyUSB0", B115200, true);
	serialSend(serial, 128);	// Send Start 
	serialSend(serial, state);	// Send state

	byte b;
	while( serialNumBytesWaiting(serial) > 0 )
		serialGetChar(serial, &b);
};

unsigned char bump(Serial* serial)
{
	unsigned char c=0;
	serialSend(serial, CmdSensors);
	serialSend(serial, 7); //bumps and wheel drops
	serialGetChar(serial, &c);
	//c &= 3;		//discard wheel drops
	return c;
};

/*
void char changeColor(Serial* serial, unsigned char c)
{
	serialSend(serial, 139);
	serialSend(serial, 4);
	serialSend(serial, c);
	serialSend(serial, 128);
};
*/

int main(void)
{
	unsigned char color = 255;
	Serial serial;
	init(&serial, 132); //full mode
	
	serialSend(&serial, 139);
	serialSend(&serial, 4);
	serialSend(&serial, color);
	serialSend(&serial, 128);

	while (true)
	{
		if ((color - 16) > 0) {
			color -= 16;
			serialSend(&serial, 139);
			serialSend(&serial, 4);
			serialSend(&serial, color);
			serialSend(&serial, 128);
		}
		sleep(1);
	}
	
	return 0;
}
