#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <phidget22.h>
#include "PhidgetHelperFunctions.h"

#define SERIAL_NO 131244 //GPS Device Serial Number (stores code 1984)

//GPS waypoint typedef
typedef struct waypoints {
	double lat;
	double lon;
	//double ms;
} Waypoint;

/*################# Function Prototypes #################*/
//Function to Calculate the desired heading to a specified destination waypoint
double findDestBearing(Waypoint current, Waypoint destination) {
	
	double dLong = (destination.lon - current.lon);	
	double X = cos(destination.lat) * sin(dLong);
	double Y = (cos(current.lat) * sin(destination.lat)) - (sin(current.lat) * cos(destination.lat) * cos(dLong));
	double bearing = atan2(X, Y);
	return bearing;
}
/*#######################################################*/


/*#################### MAIN FUNCTION ####################*/
int main() {

	//Create instance of Phidget_GPS
	PhidgetGPSHandle ch;
	PhidgetGPS_create(&ch);
	int fixState = 0;

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
	//wp0.ms	= 0.0f;
	//Create instance of PhidgetGPS_Time for log
	PhidgetGPS_Time logtime;
	//Poll GPS data Continuously
	while(1) {
		PhidgetGPS_getLatitude(ch, &wp0.lat);
		PhidgetGPS_getLongitude(ch, &wp0.lon);
		PhidgetGPS_getTime(ch, &logtime);
		PhidgetGPS_getPositionFixState(ch, &fixState);
		if (fixState == 1) {
			printf("FIX ACQUIRED\n");
		} else {
			printf("NO GPS FIX!\n");
		}
		printf("t=%d -- Lat.:%9.5fN -- Lon.:%9.5fW\n", logtime.tm_sec, wp0.lat, wp0.lon);
		printf(" Heading to Destination = %5.2f Degrees\n", findDestBearing(wp0, wp1));
		sleep(1);
		
	}
	

}
/*#######################################################*/
