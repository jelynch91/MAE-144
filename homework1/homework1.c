/*******************************************************************************
* homework1.c
*
* This is the programming portion of Homework #1 MAE 144, Fall 2017. This code
* is intended as an excercise in controlling motors based on encoder positon
*
* There are two possible modes:
*	1. Hold position - the right motor holds its position at an encoder 
*			   value of 0
*	2. Follow position - the right motor follows the encoder value of the
*			     the left motor
* The program starts in mode 1, but pressing the pause button switches the mode
* and resets the encoders. Holding the pause button ends the program
*******************************************************************************/

// usefulincludes is a collection of common system includes for the lazy
// This is not necessary for roboticscape projects but here for convenience
#include <rc_usefulincludes.h> 
// main roboticscape API header
#include <roboticscape.h>


// function declarations
void on_pause_pressed();
void on_pause_released();


/*******************************************************************************
* int main() 
*
* This template main function contains these critical components
* - call to rc_initialize() at the beginning
* - main while loop that checks for EXITING condition
* - rc_cleanup() at the end
*******************************************************************************/
int main(){
	// always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

	// initialize button functions
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);

	// initialize conversion factor c and variables for encoder readings
	// and duty cycle. c is found using the value in Part 2 and K can be
	// changed by the user
	const float c = 0.002943;
	const float k = -4;
	float rad1 = 0;
	float rad2 = 0;
	float pos_ref = 0;
	float duty = 0;

	// done initializing so set state to RUNNING and turn on the motors
	rc_set_state(RUNNING);
	rc_enable_motors();

	// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){
		
		// Get encoder position and multiply by the conversion factor
		// to get the number of radians the wheel has turned
		rad1 = rc_get_encoder_pos(2)*c;
		rad2 = rc_get_encoder_pos(3)*c;
		
		// Print to the console
		printf("Wheel 1: %f, Wheel 2: %f\n",rad1,rad2);
		

		// Initially, the program will set the reference position to
		// the angular position of the left wheel. If the pause button
		// is pressed, the program switches into mode 2 and the right
		// motor holds its position
		if (rc_get_state()==RUNNING){
			
			pos_ref = -1*rad2;
		}else if (rc_get_state()==PAUSED){
			pos_ref = 0;
		}
		

		// The duty cycle is the proportional constant k times the
		// difference between pos and pos_ref
		duty = k*(rad1-pos_ref);

		// Set the motor at the computed duty cycle
		rc_set_motor(2,duty);

		// set frequency to 100Hz
		usleep(10000);
	}
	
	// exit cleanly
	rc_cleanup();
	return 0;
}


/*******************************************************************************
* void on_pause_released() 
*	
* Make the Pause button toggle between paused and running states.
*******************************************************************************/
void on_pause_released(){
	// toggle betewen paused and running modes
	if(rc_get_state()==RUNNING){
		rc_set_state(PAUSED);
		rc_set_encoder_pos(2,0);
		rc_set_encoder_pos(3,0);
	}
	else if(rc_get_state()==PAUSED){
	rc_set_state(RUNNING);
	rc_set_encoder_pos(2,0);
	rc_set_encoder_pos(3,0);
	}
	return;
}

/*******************************************************************************
* void on_pause_pressed() 
*
* If the user holds the pause button for 2 seconds, set state to exiting which 
* triggers the rest of the program to exit cleanly.
*******************************************************************************/
void on_pause_pressed(){
	int i=0;
	const int samples = 100;	// check for release 100 times in this period
	const int us_wait = 2000000; // 2 seconds
	
	// now keep checking to see if the button is still held down
	for(i=0;i<samples;i++){
		rc_usleep(us_wait/samples);
		if(rc_get_pause_button() == RELEASED) return;
	}
	printf("long press detected, shutting down\n");
	rc_set_state(EXITING);
	return;
}
