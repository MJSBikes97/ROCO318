#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include <phidget22.h>
#include "PhidgetHelperFunctions.h"

#define SERIAL_NO 131244 //Phidget Serial. No

#define PI 3.1459f

volatile int stop = 0; //Flag to exit infinite loop

enum State {FORWARD = 0, S_LEFT = 1, H_LEFT = 2, S_RIGHT = 3, H_RIGHT = 4};

/*-------------------------------------------------------*/
//SIGINT Handler Function
void sig_handler(int signum) {
	printf("\033[10BExit!\n");
	stop = 1;
}

/*-------------------------------------------------------*/
double getTargetBearing(double lat, double lon, double tLat, double tLon) {
	double dLon = abs(tLon - lon);
	double X = cos(tLat) * sin(dLon);
	double Y = (cos(lat) * sin(tLat)) - (sin(lat) * cos(tLat) * cos(dLon));
	return (atan2(X, Y) * (180.0f/PI)) + 180.0f;
}

/*-------------------------------------------------------*/
double getBearingError(double head, double bearing) {
	return bearing - head;
}
/*-------------------------------------------------------*/
State setState(double error) {
	State stateVar;
	switch (error) {
		case (error < 10.0f | error >= 350.0f):
			stateVar = FORWARD;
		break;
		case (90.0f > error >= 10.0f):
			stateVar = S_LEFT;
		break;
		case (180.0f > error >= 90.0f):
			stateVar = H_LEFT;
		break;
		case (270.0f > error >= 180.0f):
			stateVar = H_RIGHT;
		break;	
		case (350.0f > error >= 270.0f):
			stateVar = S_RIGHT;
		break;
		default:
			stateVar = FORWARD;
		break;
		
	}	
	return stateVar;
}
/*-------------------------------------------------------*/

int main() {

	signal(SIGINT, sig_handler);	

	FILE *fp =  fopen("myGPS_data.csv", "w");
	
	//Create Variables for position data
	double lat = 0.0f;
	double lon = 0.0f;
	double tLat = 50.364351f;
	double tLon = -4.141873f;	
	
	//Create Variables for Heading Data
	double head = 0.0f;
	double bearingToTarget = 0.0f;
	double error = 0.0f;

	PhidgetGPSHandle myGPS; //Name for GPS device
	PhidgetGPS_create(&myGPS);

	Phidget_setDeviceSerialNumber((PhidgetHandle)myGPS, SERIAL_NO);
	Phidget_openWaitForAttachment((PhidgetHandle)myGPS, 5000); //Open channel and wait up to 5s
	//Create State Variable for Turning
	State STATE = FORWARD;
	//Enter file header info
	//fprintf(fp,"myGPS_data2.csv\n");
	fprintf(fp, "lat,lon\n");
	//printf("#########################\nLocation:\033[s\n#########################\nHeading:\nBearing:\nError:\n#########################\033[u");
	while(!stop) {
		//Get Positional Data
		PhidgetGPS_getLatitude(myGPS, &lat);
		PhidgetGPS_getLongitude(myGPS, &lon);
		//Get Heading Data
		PhidgetGPS_getHeading(myGPS, &head);
		bearingToTarget = getTargetBearing(lat, lon, tLat, tLon);
		error = bearingToTarget - head;
		//Printout Positional Data
		fprintf(fp,"%9.7f,%9.7f\n", lat, lon);
		printf("--------------------------------------\nLocation: %9.7f N %9.7f W\n--------------------------------------\nHeading: %5.2f \nTarget Bearing: %5.2f \nError:%5.2f\033[5A", lat, lon, head, bearingToTarget, error);
		usleep(100000);
	}
	fclose(fp);
	return 0;
	
}
