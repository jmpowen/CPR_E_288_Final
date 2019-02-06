/**
* @file movement.c
* @brief Functions for controlling cybot's movements.
*
* @author Sarah Baratta
* @author Chen Ye Lim
* @date 09/04/18
*/

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include <stdio.h>

void move_forward(oi_t *sensor, int millimeters);
void turn_clockwise(oi_t *sensor, int degrees);
void turn_anticlockwise(oi_t *sensor, int degrees);
void move_backward(oi_t *sensor, int millimeters);

#endif /* MOVEMENT_H_ */
