/**
* @file servo.h
* @brief Functions for moving Servo.
*
* @author Matt Johnson
* @author Thomas Kirby
* @author John Powen
* @date 12/7/2018
*/


#ifndef SERVO_H_
# define SERVO_H_

void servo_init(void);
void servo_move(unsigned pusle_width);
int getCount();

#endif
