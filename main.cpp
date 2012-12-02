//============================================================================
// Name        : ptproxy.cpp
// Author      : wacek
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include "ptproxy/PTProxy.h"
using namespace std;

int main() {
	PTProxy *ptp = new PTProxy("/dev/ttyACM0");

	char inp;
	float dx = 0, dy = 0;

	while(1){
		cout << "error: " << dx << ", " << dy << endl;
		cin >> inp;
		switch(inp){
		case 'a':
			dx -= 1;
			ptp->feedError(dx, dy);
			break;
		case 'd':
			dx += 1;
			ptp->feedError(dx, dy);
			break;
		case 's':
			dy -= 1;
			ptp->feedError(dx, dy);
			break;
		case 'w':
			dy += 1;
			ptp->feedError(dx, dy);
			break;
		case 'x':
			ptp->nextStep();
			break;
		default:
			return 0;
		}
		ptp->nextStep();
	}
	return 0;
}
