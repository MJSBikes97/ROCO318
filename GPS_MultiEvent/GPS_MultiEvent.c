#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <ctype.h>
#include <phidget22.h>
#include "PhidgetHelperFunctions.h"

#define SERIAL_NO 131244 //GPS Device Serial Number (stores code 1984)
#define PI 3.1459f 

volatile int stop = 0;

//GPS waypoint typedef
typedef struct waypoints {
	double lat;
	double lon;
	//double ms;
} Waypoint;

/*################# Function Prototypes #################*/
//Function to Calculate the desired heading to a specified destination waypoint
double findDestBearing(Waypoint current, Waypoint destination) {
	//All values must be converted to radians for use with math library trig functions
	double dLong = abs(destination.lon - current.lon);	
	double X = cos(destination.lat) * sin(dLong);
	double Y = (cos(current.lat) * sin(destination.lat)) - (sin(current.lat) * cos(destination.lat) * cos(dLong));
	double bearing = atan2(X, Y) * 180.0f/PI;
	return bearing+180.0f;
}
/*-------------------------------------------------------*/
double getBearingError(double heading, double bearing) { 
	//double heading = 0.0f;
	//PhidgetGPS_getHeading(ch, &heading);	
	double error = bearing - heading;
	return error;
}
/*-------------------------------------------------------*/
void createLogFile(FILE *logfile, char filename[]) {
	filename = strcat(filename, ".csv"); //Attach csv extension to filename variable
	logfile = fopen(filename, "w"); //Create logfile with filename specified
	if (!logfile) {
		printf("fopen failed! -- No logging...");
	}
	fprintf(logfile, "lat, long\n"); //Print column headings
}
/*-------------------------------------------------------*/
void printLogFile(FILE *logfile, Waypoint current) {
	fprintf(logfile, "%9.7f,%9.7f\n", current.lat, current.lon);
}
/*-------------------------------------------------------*/
void closeLogFile(FILE *logfile) {
	//fflush(logfile);
	if(!logfile) {
		printf("Error! -- Cannot Close logfile!");
	} else {
		fclose(logfile);
	}
}
/*-------------------------------------------------------*/
void signal_callback_handler(int signum) {
	printf("\nSampling Interrupted!\n");
	stop = 1;	
}
/*#######################################################*/


/*#################### MAIN FUNCTION ####################*/
int main(int argc, char *argv[]) {
	signal(SIGINT, &signal_callback_handler); //initialise exit sigint		

	//Create instance of Phidget_GPS
	PhidgetGPSHandle ch;
	PhidgetGPS_create(&ch);

	FILE *GPSLog; //File for logging position data
	//Initialise variables
	int fixState = 0;
	double heading = 0.0f;
	double bearing = 0.0f;
	double error = 0.0f;

	char logname[] = "log1";
	createLogFile(GPSLog, logname);

	//Specify Serial No.
	Phidget_setDeviceSerialNumber((PhidgetHandle)ch, SERIAL_NO);
	
	//Attach Event Handlers for Pos. and Heading Changes

	//Open channel and wait 5s for device attachment
	Phidget_openWaitForAttachment((PhidgetHandle)ch, 5000);
	
	//Create instance of Waypoint for Destination Heading Calculation
	Waypoint wp1;
	wp1.lat = 50.364351f;
	wp1.lon = -4.141873f;

	//Create instance of Waypoint for storing GPS info
	Waypoint wp0;
	wp0.lat = 0.0f;
	wp0.lon = 0.0f;

	//Create instance of PhidgetGPS_Time for log
	PhidgetGPS_Time logtime;
	
	//Poll GPS data Continuously
	while(!stop) {
		//Gather Data from GPS device
		//GPSLog = fopen("log1.csv", "w");
		PhidgetGPS_getLatitude(ch, &wp0.lat);
		PhidgetGPS_getLongitude(ch, &wp0.lon);
		PhidgetGPS_getHeading(ch, &heading);
		PhidgetGPS_getTime(ch, &logtime);
		PhidgetGPS_getPositionFixState(ch, &fixState);
		//Calculate Bearing to target
		bearing = findDestBearing(wp0, wp1);
		error = getBearingError(heading, bearing);
		//Output GPS Fix state
		if (fixState == 1) {
			printf("FIX ACQUIRED\n");
		} else {
			printf("NO GPS FIX!\n");
		}
		//Print Data to Logfile
		//printLogFile(GPSLog, wp0);
		fprintf(GPSLog, "%9.7f,%9.7f\n", wp0.lat, wp0.lon);
		//fprintf(GPSLog, "%9.7f,%9.7f\n", wp0.lat, wp0.lon);

		//Print GPS position to Terminal
		printf("t=%d -- Lat.:%9.5fN -- Lon.:%9.5fW\n", logtime.tm_sec, wp0.lat, wp0.lon);
		//Print Heading and Error information to Terminal
		printf(" Heading to Destination = %5.2f Degrees\n", bearing);
		printf("Actual Heading = %5.2f Degrees -- Error = %5.2f Degrees\n", heading, error);
		//fclose(GPSLog);
		sleep(1); //Polling Delay 1s
		
	}

	closeLogFile(GPSLog); //runs Fclose
	printf("Cleaning up...\n");
	Phidget_close((PhidgetHandle)ch);
	PhidgetGPS_delete(&ch);
	printf("\nExiting...\n");

	return 0;
}
/*#######################################################*/
