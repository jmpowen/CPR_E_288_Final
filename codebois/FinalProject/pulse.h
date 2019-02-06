/**
* @file pulse.h
* @brief Functions for creating a pulse using the PWM.
*
* @author Matt Johnson
* @author Thomas Kirby
* @author John Powen
* @date 10/20/2018
*/


#ifndef PULSE_H_
# define PULSE_H_

//unsigned time2dist(unsigned time);
void send_pulse();
//void TIMER3B_Handler(void);
//unsigned ping_read();
int pulse_init(void);

int getIsDone();

int time2dist();

int getOver();

#endif
