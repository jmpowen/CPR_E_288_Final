/**
* @file movement.c
* @brief Detects bumps and gives direction to control cybot's movements.
*
* @author Sarah Baratta
* @author Chen Ye Lim
* @date 09/04/2018
*/

#include "openinterface.h"

void go(oi_t *sensor, int millimeters)
{
    int sum = 0;
    oi_setWheels(200, 200);                //setting full speed

    while (sum < millimeters) {
        oi_update(sensor);                 //going forward
        sum += sensor->distance;

        if (sensor->bumpLeft) {            //detects bump on left

            oi_setWheels(0,0);             //stop
            move_backward(sensor, 150);    //back 15cm
            turn_anticlockwise(sensor, 90);//turn left
            move_forward(sensor, 250);     //forward 15cm
            turn_clockwise(sensor, 90);    //turn right
            go(sensor, millimeters+150-sum);          //FINISH 2m
            oi_setWheels(0, 0);            //stop
        }

        if (sensor->bumpRight) {

            oi_setWheels(0,0);             //stop
            move_backward(sensor, 150);    //back 15cm
            turn_clockwise(sensor, 90);    //turn right
            move_forward(sensor, 250);     //forward 15cm
            turn_anticlockwise(sensor, 90);//turn left
            go(sensor, millimeters+150-sum);
            oi_setWheels(0, 0);            //stop
        }
    }

    oi_setWheels(0, 0);//stop
    //oi_free(sensor);
}


void turn_clockwise(oi_t *sensor, int degrees)
{
    oi_setWheels(-100, 100);
    while (degrees > 0) //turn right by 90 degree
    {
        oi_update(sensor);
        degrees += sensor->angle;
    }

    oi_setWheels(0, 0); // stop

}

void turn_anticlockwise(oi_t *sensor, int degrees)
{
    oi_setWheels(100, -100);
    while (degrees > 0) //turn right by 90 degree
    {
        oi_update(sensor);
        degrees -= sensor->angle;
    }

    oi_setWheels(0, 0); // stop

}

void move_forward(oi_t *sensor, int millimeters)
{
    int sum = 0;
    oi_setWheels(100,100); // move forward; full speed

    while (sum < millimeters) {
        oi_update(sensor);
        sum += sensor->distance;
    }

    oi_setWheels(0, 0); // stop
    //oi_free(sensor);
}

void move_backward(oi_t *sensor, int millimeters)
{
    int sum=0;
    oi_setWheels(-100, -100);    //back direction

    while (sum > -millimeters) {
        oi_update(sensor);
        sum += sensor->distance;
    }

    oi_setWheels(0, 0); // stop
}
