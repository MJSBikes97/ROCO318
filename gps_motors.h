#ifndef GPS_MOTORS_h_
#define GPS_MOTORS_h_

//Left motor pin assignments
#define L_Dir1 0
#define L_Dir2 2
#define L_En   3

//Right motor pin assignments
#define R_Dir1 8
#define R_Dir2 9
#define R_En   7

//Motor initialisation function
void Motors_Init(void);

//Motor control functions
void Motors_Disable();
void Forwards(int intensity);
void Backwards(int intensity);
void Hard_Left();
void Hard_Right();
void Smooth_Turn(int intensityL, int intensityR);

#endif
