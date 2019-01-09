/*---------------------------------------------------------------------------------------------------------/
Source Name: gps_motors.c
Source Description: Functions to drive the motors in various diractions
/---------------------------------------------------------------------------------------------------------*/

#include <wiringPi.h>
#include <softPwm.h>
#include "gps_motors.h"

  /* motor driver truth table
   
    Dir1  |  Dir2  | Enable | Result
     X    |    X   |    0   | Motor disabled
     0    |    0   |    1   | Motor off
     0    |    1   |    1   | Motor clockwise
     1    |    0   |    1   | Motor anti-clockwise
     1    |    1   |    1   | Motor off
   */

/*---------------------------------------------------------------------------------------------------------/
Function Name: Motors_Disable
Function Description: Disables the motor enable pins
Input Parameters: N/A
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void Motors_Disable(){

 //Left Motor Off
 digitalWrite (L_Dir1, LOW); 
 digitalWrite (L_Dir2, LOW);
 softPwmWrite (L_En, 0);	//PWM enable Off

 //Right Motor Off
 digitalWrite (R_Dir1, LOW);
 digitalWrite (R_Dir2, LOW);
 softPwmWrite (R_En, 0);	//PWM enable off

}

/*---------------------------------------------------------------------------------------------------------/
Function Name: Forwards
Function Description: Drives both motors forwards
Input Parameters: Intensity - the duty cycle of the motors. No PWM at 100%
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void Forwards(int intensity){

 //Left Motor Forwards
 digitalWrite (L_Dir1, LOW);
 digitalWrite (L_Dir2, HIGH);
 softPwmWrite (L_En, intensity); //PWM enable @ 'intensity'%

 //Right Motor Forwards
 digitalWrite (R_Dir1, LOW);
 digitalWrite (R_Dir2, HIGH);
 softPwmWrite (R_En, intensity); //PWM enable @ 'intensity'%

}

/*---------------------------------------------------------------------------------------------------------/
Function Name: Backwards
Function Description: Drives both motors backwards
Input Parameters: Intensity - the duty cycle of the motors. No PWM at 100%
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void Backwards(int intensity){

 //Left Motor Backwards
 digitalWrite (L_Dir1, HIGH);
 digitalWrite (L_Dir2, LOW);
 softPwmWrite (L_En, intensity); //PWM enable @ 'intensity'%

 //Right Motor Backwards
 digitalWrite (R_Dir1, HIGH);
 digitalWrite (R_Dir2, LOW);
 softPwmWrite (R_En, intensity); //PWM enable @ 'intensity'%

}

/*---------------------------------------------------------------------------------------------------------/
Function Name: Hard_Left
Function Description: A sharp, on the spot left turn involving both motors
Input Parameters: N/A
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void Hard_Left(){

 //Left Motor Forwards
 digitalWrite (L_Dir1, LOW);
 digitalWrite (L_Dir2, HIGH);
 softPwmWrite (L_En, 100); //PWM enable @ 100%

 //Right Motor Backwards
 digitalWrite (R_Dir1, HIGH);
 digitalWrite (R_Dir2, LOW);
 softPwmWrite (R_En, 100); //PWM enable @ 100%

}

/*---------------------------------------------------------------------------------------------------------/
Function Name: Hard_Right
Function Description: A sharp, on the spot right turn involving both motors
Input Parameters: N/A
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void Hard_Right(){

 //Left Motor Backwards
 digitalWrite (L_Dir1, HIGH);
 digitalWrite (L_Dir2, LOW);
 softPwmWrite (L_En, 100); //PWM enable @ 100%

 //Right Motor Forwards
 digitalWrite (R_Dir1, LOW);
 digitalWrite (R_Dir2, HIGH);
 softPwmWrite (R_En, 100); //PWM enable @ 100%

}

/*---------------------------------------------------------------------------------------------------------/
Function Name: Smooth_Turn
Function Description: A turning function where the duty cycle of each motor can be set independantly for a smoother turn
Input Parameters: IntensityL, IntensityR - the duty cycle of the left and right motor respectively. No PWM at 100%
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void Smooth_Turn(int intensityL, int intensityR){

 //Left Motor Forwards
 digitalWrite (L_Dir1, LOW);
 digitalWrite (L_Dir2, HIGH);
 softPwmWrite (L_En, intensityL); //PWM enable @ 'intensityL'%

 //Right Motor Forwards
 digitalWrite (R_Dir1, LOW);
 digitalWrite (R_Dir2, HIGH);
 softPwmWrite (R_En, intensityR); //PWM enable @ 'intensityR'%

}

/*---------------------------------------------------------------------------------------------------------/
Function Name: Motors_Init
Function Description: Sets up the rapberry pi pins to control the motor driver
Input Parameters: N/A
Output Parameters: N/A
/---------------------------------------------------------------------------------------------------------*/
void Motors_Init(void){
  	
  wiringPiSetup (); //Initialises wiringPi pin mapping
  
  pinMode (L_Dir1, OUTPUT); //Left Motor Drive 1
  pinMode (L_Dir2, OUTPUT); //Left Motor 1 Drive 2
  softPwmCreate(L_En, 0, 100); //Left Motor 1 Enable
  
  pinMode (R_Dir1, OUTPUT); //Right Motor 2 Drive 1
  pinMode (R_Dir2, OUTPUT); //Right Motor 2 Drive 2
  softPwmCreate(R_En, 0, 100); //Left Motor 1 Enable
  
/*------------------------------------MOTOR TEST FUNCTION-------------------------------------------------*/
  /* while(1){
  
    for(int i = 0; i < 100; i++){
  	Smooth_Turn(i, 100 - i);
  	delay(10);
    }
    
    delay(1000);
    
    for(int i = 0; i < 100; i++){
  	Smooth_Turn(100 - i, i);
  	delay(10);
    }
    
    delay(1000);
    
  }
  
  */
}

