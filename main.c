/*---------------------------------------------------------------------------------------------------------/
Source Name: main.c
Source Description: Main code of GPS robot
/---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "gps_motors.h"

#include <phidget22.h>
#include "PhidgetHelperFunctions.h"

#define SERIAL_NO 131244 //Phidget Serial. No

#define PI 3.1459f //Mathematical operator

#define FullSpeed 100
#define TurnSpeed 80
#define StopSpeed 0

volatile int stop = 0; //Flag to exit infinite loop


/*---------------------------------------------------------------------------------------------------------/
Function Name: sig_handler
Function Description: Sets a signal upon closing application which allows us to manage how we close the program
Input Parameters: signum
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void sig_handler(int signum) {
	printf("\033[10BExit!\n");
	stop = 1;
}

/*---------------------------------------------------------------------------------------------------------/
Function Name: getTargetBearing
Function Description: Calculates bearing to target based on the latitute and longitude data of the robot and the target
Input Parameters: lat, lon (Robot Lat, long values), tlat, tlon (Target Lat, Long values)
Output Parameters:The target bearing
/---------------------------------------------------------------------------------------------------------*/
double getTargetBearing(double lat, double lon, double tLat, double tLon) {
	double dLon = abs(tLon - lon);
	double X = cos(tLat) * sin(dLon);
	double Y = (cos(lat) * sin(tLat)) - (sin(lat) * cos(tLat) * cos(dLon));
	return (atan2(X, Y) * (180.0f/PI)) + 180.0f;
}

/*---------------------------------------------------------------------------------------------------------/
Function Name: getBearingError
Function Description: Calculates the error value between the current bearing and the target bearing
Input Parameters: head is the target heading, bearing is the robots current bearing 
Output Parameters: Bearing error value
/---------------------------------------------------------------------------------------------------------*/

double getBearingError(double head, double bearing) {
	return bearing - head;
}

/*---------------------------------------------------------------------------------------------------------/
Function Name: set_turnmode
Function Assigns the direction and intensity of the motors based on the error bearing between the robot and the waypoint 
Input Parameters: f_error - The bearing error between the robot and the waypoint
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void set_turnmode(double f_error){
	f_error = f_error*10.0f;	
	int error = (int)f_error; //typecast to int for comparison
	printf("\nerror: %d\n", error);
	if (error < 100 || error >= 3500) {
		Forwards(FullSpeed);
 		printf("\nForwards\n");
	} else if (error < 900 && error >= 100) {
		Smooth_Turn(FullSpeed , TurnSpeed); 
		printf("\nSmooth Left\n");
	} else if (error < 1800 && error >= 900) {
		Hard_Left(); 				
		printf("\nHard Left\n");
	} else if (error < 2700 && error >= 1800) {
		Hard_Right(); 		
		printf("\nHard Right\n");
	} else if (error < 3500 && error >= 2700) {
		Smooth_Turn(TurnSpeed, FullSpeed); 	
		printf("\nSmooth Right\n");
	} else {
		Motors_Disable(); 	
		printf("\nout of range!!!\n");		//Default off
	
	}


}


/*---------------------------------------------------------------------------------------------------------/
Function Name: Main
Function Description: Main application routine
Input Parameters: N/A 
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
int main() {

	//Setup interrupt on closing application with Ctrl + C
	signal(SIGINT, sig_handler);	

	//Open the file
	FILE *fp =  fopen("myGPS_data.csv", "w");
	
	//Create Variables for position data
    double lat = 0.0f;      //Latitude
    double lon = 0.0f;      //Longitude
    double tLat = 50.364351f;   //Target Latitude
    double tLon = -4.141873f;   //Target Longitude
	
	
	//Create Variables for Heading Data
    double head = 0.0f;     //Heading
    double bearingToTarget = 0.0f;  //Bearing to target
    double error = 0.0f;        //Bearing error between robot and target

	//Initialise motors
	Motors_Init(); 
	
	//Name and address for GPS device
	PhidgetGPSHandle myGPS; 
	PhidgetGPS_create(&myGPS);

	 //Obtain the GPS device's serial number and open communication chanel, 5 second timeout
	Phidget_setDeviceSerialNumber((PhidgetHandle)myGPS, SERIAL_NO);
	Phidget_openWaitForAttachment((PhidgetHandle)myGPS, 5000); 
	
	//Enter file header info
	fprintf(fp,"myGPS_data.csv\n");
	fprintf(fp, "lat,lon\n");

/*--------------------------------------------MAIN WHILE LOOP---------------------------------------------*/	
	while(!stop) {

		//Get Positional Data
		PhidgetGPS_getLatitude(myGPS, &lat);
		PhidgetGPS_getLongitude(myGPS, &lon);

		//Get Heading Data
		PhidgetGPS_getHeading(myGPS, &head);
		bearingToTarget = getTargetBearing(lat, lon, tLat, tLon);
		error = abs(bearingToTarget - head);

		//print positional data to file and serial terminal
		fprintf(fp,"%9.7f,%9.7f\n", lat, lon);
		printf("--------------------------------------\nLocation: %9.7f N %9.7f W\n--------------------------------------\nHeading: %5.2f \nTarget Bearing: %5.2f \nError:%5.2f\033[5A", lat, lon, head, bearingToTarget, error);
		printf("\nHeading Error: %5.2f\n", error);
		printf("\nHeading: %5.2f\n", head);
		set_turnmode(error);
		usleep(100);
	}

	//Close file to ensure buffer is successfully emptied on close & disable the motors
	fclose(fp);
	Motors_Disable();

	return 0;
	
}
