//============================================================================
// Name        : ptproxy.cpp
// Author      : wacek
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <stdio.h>
#include <cstring>
#include "ptproxy/PTProxy.h"
using namespace std;

int main() {
	PTProxy *ptp = new PTProxy("/dev/ttyACM0");

	char inp;
	float dx = 0, dy = 0, rx, ry;
	bool synchronized = false;
	int kfd = 0;
	struct termios cooked, raw;

	// get the console in raw mode
	tcgetattr(kfd, &cooked);
	memcpy(&raw, &cooked, sizeof(struct termios));
	raw.c_lflag &= ~(ICANON);
	raw.c_lflag &= ~(ECHO);
	raw.c_cc[VEOL] = 1;
	raw.c_cc[VEOF] = 2;
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;
	tcsetattr(kfd, TCSANOW, &raw);
	// read() is non-blocking from now on and echo is off

	// Prepare synchronization mode request
	ptp->startSynchronization();
	// Send it to motor controllers
	ptp->nextStep();

	printf("Pan-tilt speed control: [A] x-  [D] x+  [S] y-  [W] y+  [space] stop\n\n");
	printf("Quit: [Q]\n\n");
	sleep(1);

	bool quit = false;
	while(!quit){
		// Communication cycle interval is 100ms
		usleep(100000);

		// Prepare data to send
		if(read(0, (void*)&inp, 1)){
			switch(inp){
			case 'a':
			case 'A':
				dx -= 1;
				ptp->setMotorSpeed(dx, dy);
				break;
			case 'd':
			case 'D':
				dx += 1;
				ptp->setMotorSpeed(dx, dy);
				break;
			case 's':
			case 'S':
				dy -= 1;
				ptp->setMotorSpeed(dx, dy);
				break;
			case 'w':
			case 'W':
				dy += 1;
				ptp->setMotorSpeed(dx, dy);
				break;
			case ' ':
				dx = 0;
				dy = 0;
				ptp->setMotorSpeed(dx, dy);
				break;
			case 'q':
			case 'Q':
				ptp->setMotorSpeed(0, 0);
				quit = true;
				break;
			}
		}
		// Read - write hardware
		ptp->nextStep();
		// Process received data
		ptp->getMotorPosition(rx, ry);
		printf("px: %6d, py: %6d, syn: %d\n", (int)rx, (int)ry, ptp->isSynchronized());


	}

	// restore console settings
	raw.c_lflag |= ECHO | ICANON;
	raw.c_cc[VEOL] = 1;
	raw.c_cc[VEOF] = 2;
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;
	tcsetattr(kfd, TCSANOW, &raw);

	return 0;
}
