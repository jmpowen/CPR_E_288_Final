/**
* @file move.c
* @brief Contains the main for running the final
* project. Combines Putty, move, and scan.
*
* @author Sarah Baratta
* @author Matt Johnson
* @author Thomas Kirby
* @author Chen Ye Lim
* @author John Powen
* @date 12/7/2018
*/

#include <time.h>
#include "openinterface.h"
#include "movement.h"
#include "uart.h"
#include "timer.h"

void check_sensors(oi_t *sensor);
void move_forwards(oi_t *sensor);
void move_backwards(oi_t *sensor);
void turn_counter_Clockwise(oi_t *sensor);
void turn_clockwise_(oi_t *sensor);

void main ()
{
    oi_t *sensor = oi_alloc();
    oi_init(sensor);
    servo_init();
    char dir=0;
    int move=0;
    int turn=0;

    while (1)
    {
        //check_sensors(sensor);

        uart_init();
        dir = uart_receive()+'0'-'0'; //receive direction command
        timer_waitMillis(100);
        uart_sendChar('\n');
        uart_sendChar('\r');


        if (dir == 'w')
        {
            move = 1;
           // if (move >= 1) move = 1; //limit max value for move is 1
            turn=0;
        }
        else if (dir == 's')
        {
            move = -1;
            //if (move <= -1) move = -1; //limit min value for move is -1
            turn=0;
        }
        else if (dir == 'a')
        {
            turn = 1;
          //  if (turn >= 1) turn = 1;  //limit max value for turn is 1
            move=0;
        }
        else if (dir == 'd')
        {
            turn = -1;
            //if (turn <= -1) turn = -1; //limit min value for turn is -1
            move=0;
        }
        else if (dir == 'x')
        {
            turn =0;    // stop bot
            move=0;
        }
        else if(dir == 'q')
        {
            scan();
            turn = 0;
            move = 0;
        }
        goRobot(move, turn, sensor);    //free data
        timer_waitMillis(500);

    }

    oi_free(sensor);
}
/**
 * This method will be used to check all of the sensors. This later became integrated into move_forward method. This code is redundant.
 * @author John Powen, Sarah Baratta
 * @param sensor the sensor that will be checked for updates.
 * @date 12/7/2018
 */
void check_sensors(oi_t *sensor)
{

    oi_update(sensor);
    /*
     * Checks the bumpers
     */
    if(sensor->bumpLeft && sensor->bumpRight)
    {
        char *conv_disp = "#11#";
        uart_sendStr(conv_disp);
    }
    else if(sensor->bumpLeft)
    {
        char *conv_disp = "#10#";
        uart_sendStr(conv_disp);
    }
    else if(sensor->bumpRight)
    {
        char *conv_disp = "#01#";
        uart_sendStr(conv_disp);
    }

    /*
     * Checks for cliffs
     */
    if(sensor->cliffFrontLeft && sensor->cliffFrontRight)
    {
        char *conv_disp = "!0110!";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffFrontLeft)
    {
        char *conv_disp = "!0100!";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffFrontRight)
    {
        char *conv_disp = "!0010!";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffRight)
    {
        char *conv_disp = "!0001!";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffLeft)
    {
        char *conv_disp = "!1000!";
        uart_sendStr(conv_disp);
    }

    /*
     * Check for tape
     */
    if(sensor->cliffFrontLeftSignal > 2750 && sensor->cliffFrontRightSignal > 2750)
    {
        char *conv_disp = "%0110%";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffFrontLeftSignal > 2750)
    {
        char *conv_disp = "%0100%";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffFrontRightSignal > 2750)
    {
        char *conv_disp = "%0010%";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffRightSignal > 2750)
    {
        char *conv_disp = "%0001%";
        uart_sendStr(conv_disp);
    }
    else if(sensor->cliffLeftSignal > 2750)
    {
        char *conv_disp = "%1000%";
        uart_sendStr(conv_disp);
    }

    /*
     * URGENT: CHECK WHEEL DROP
     */
    if(sensor->wheelDropLeft)
    {
        char *conv_disp = "!!10!!";
        uart_sendStr(conv_disp);
    }
    else if(sensor->wheelDropRight)
    {
        char *conv_disp = "!!01!!";
        uart_sendStr(conv_disp);
    }
}

/**
 * This method will handle calling the correct moving and turning methods.
 * @author Chenye Lim
 * @param move if non-zero then move forward or backwards
 * @param turn if non-zero then move right or left
 * @param sensor the sensor being passed to the move functions
 * @date 12/7/2018
 */
void goRobot(int move, int turn, oi_t *sensor){

    if (turn!=0) {
        switch (turn) {
            case -1:  turn_clockwise_(sensor); break;       //turn right
            case 1:  turn_counter_Clockwise(sensor); break; //turn left
            case 0:  oi_setWheels(0,0); break;              //stop regardless
            }
        }
    else {
        switch (move) {
            case 1:  move_forwards(sensor); break;          //move forward
            case -1:    move_backwards(sensor); break;      //move backward
            case 0:  oi_setWheels(0,0); break;              //stop regardless
            }
        }
}

/**
 * This method moves forwards while always checking for any hits on the sensors.
 * @author John Powen, Matt Johnson, Tom Kirby
 * @param sensor the sensor always checked for updates
 * @date 12/7/2018
 */
void move_forwards(oi_t *sensor)
{
    int distance = 0;
    int flag = 0;
    char disp[10];
    while(distance < 100)
    {
        oi_update(sensor);
        /*
         * Checks the bumpers
         */
        if (sensor->bumpLeft && sensor->bumpRight)
        {
            char *conv_disp = "#11#";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->bumpLeft)
        {
            char *conv_disp = "#10#";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->bumpRight)
        {
            char *conv_disp = "#01#";
            uart_sendStr(conv_disp);
            flag = 1;
        }


        /*
         * Checks for cliffs
         */
        if (sensor->cliffFrontLeft && sensor->cliffFrontRight)
        {
            char *conv_disp = "!0110!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontLeft)
        {
            char *conv_disp = "!0100!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontRight)
        {
            char *conv_disp = "!0010!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffRight)
        {
            char *conv_disp = "!0001!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffLeft)
        {
            char *conv_disp = "!1000!";
            uart_sendStr(conv_disp);
            flag = 1;
        }

        /*
         * Check for tape
         */
        if (sensor->cliffFrontLeftSignal > 2750
                && sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0110%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontLeftSignal > 2750)
        {
            char *conv_disp = "%0100%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0010%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffRightSignal > 2750)
        {
            char *conv_disp = "%0001%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffLeftSignal > 2750)
        {
            char *conv_disp = "%1000%";
            uart_sendStr(conv_disp);
            flag = 1;
        }

        /*
         * URGENT: CHECK WHEEL DROP
         */
        if (sensor->wheelDropLeft)
        {
            char *conv_disp = "!!10!!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->wheelDropRight)
        {
            char *conv_disp = "!!01!!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        distance += sensor->distance;
        if(flag)
        {
            oi_setWheels(-100,-100);
            timer_waitMillis(350);
            oi_setWheels(0,0);
            break;
        }
        else
        {
            oi_setWheels(124,98); //slightly to the right

        }
    }
    int temp = distance;
    if(flag) {
        temp -= 20;
    }

    sprintf(disp,"^%d^", temp);
    uart_sendStr(disp);
    oi_setWheels(0,0);
}

/**
 * This method moves backwards while always checking for any hits on the sensors.
 * @author John Powen, Matt Johnson, Tom Kirby
 * @param sensor the sensor always checked for updates
 * @date 12/7/2018
 */
void move_backwards(oi_t *sensor)
{
    int distance = 0;
    int flag = 0;
    char disp[10];
    while(distance < 100)
    {
        oi_update(sensor);
        /*
         * Checks the bumpers
         */
        if (sensor->bumpLeft && sensor->bumpRight)
        {
            char *conv_disp = "#11#";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->bumpLeft)
        {
            char *conv_disp = "#10#";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->bumpRight)
        {
            char *conv_disp = "#01#";
            uart_sendStr(conv_disp);
            flag = 1;
        }

        /*
         * Checks for cliffs
         */
        if (sensor->cliffFrontLeft && sensor->cliffFrontRight)
        {
            char *conv_disp = "!0110!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontLeft)
        {
            char *conv_disp = "!0100!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontRight)
        {
            char *conv_disp = "!0010!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffRight)
        {
            char *conv_disp = "!0001!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffLeft)
        {
            char *conv_disp = "!1000!";
            uart_sendStr(conv_disp);
            flag = 1;
        }

        /*
         * Check for tape
         */
        if (sensor->cliffFrontLeftSignal > 2750
                && sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0110%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontLeftSignal > 2750)
        {
            char *conv_disp = "%0100%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0010%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffRightSignal > 2750)
        {
            char *conv_disp = "%0001%";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->cliffLeftSignal > 2750)
        {
            char *conv_disp = "%1000%";
            uart_sendStr(conv_disp);
            flag = 1;
        }

        /*
         * URGENT: CHECK WHEEL DROP
         */
        if (sensor->wheelDropLeft)
        {
            char *conv_disp = "!!10!!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        else if (sensor->wheelDropRight)
        {
            char *conv_disp = "!!01!!";
            uart_sendStr(conv_disp);
            flag = 1;
        }
        distance -= sensor->distance;
        if (flag)
        {
            oi_setWheels(0, 0);
            sprintf(disp, "\r\n^%d^\r\n", distance);
            uart_sendStr(disp);
            break;
        }
        else
        {
            oi_setWheels(-100, -100);
        }
    }
    oi_setWheels(0,0);
}

/**
 * This method turns clockwise while always checking for any hits on the sensors.
 * @author Sarah Baratta, Chenye Lim
 * @param sensor the sensor always checked for updates
 * @date 12/7/2018
 */
void turn_clockwise_(oi_t *sensor) {
    int degrees = 44;
    while (degrees > 0) //turn right by 45 degrees
    {
        oi_update(sensor);
        /*
         * Checks the bumpers
         */
        if (sensor->bumpLeft && sensor->bumpRight)
        {
            char *conv_disp = "#11#";
            uart_sendStr(conv_disp);
        }
        else if (sensor->bumpLeft)
        {
            char *conv_disp = "#10#";
            uart_sendStr(conv_disp);
        }
        else if (sensor->bumpRight)
        {
            char *conv_disp = "#01#";
            uart_sendStr(conv_disp);
        }

        /*
         * Checks for cliffs
         */
        if (sensor->cliffFrontLeft && sensor->cliffFrontRight)
        {
            char *conv_disp = "!0110!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontLeft)
        {
            char *conv_disp = "!0100!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontRight)
        {
            char *conv_disp = "!0010!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffRight)
        {
            char *conv_disp = "!0001!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffLeft)
        {
            char *conv_disp = "!1000!";
            uart_sendStr(conv_disp);
        }

        /*
         * Check for tape
         */
        if (sensor->cliffFrontLeftSignal > 2750
                && sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0110%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontLeftSignal > 2750)
        {
            char *conv_disp = "%0100%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0010%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffRightSignal > 2750)
        {
            char *conv_disp = "%0001%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffLeftSignal > 2750)
        {
            char *conv_disp = "%1000%";
            uart_sendStr(conv_disp);
        }

        /*
         * URGENT: CHECK WHEEL DROP
         */
        if (sensor->wheelDropLeft)
        {
            char *conv_disp = "!!10!!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->wheelDropRight)
        {
            char *conv_disp = "!!01!!";
            uart_sendStr(conv_disp);
        }

        degrees += sensor->angle;
        oi_setWheels(-100, 100);
    }
     oi_setWheels(0, 0); // stop

}

/**
 * This method turns clockwise while always checking for any hits on the sensors.
 * @author Sarah Baratta, Chenye Lim
 * @param sensor the sensor always checked for updates
 * @date 12/7/2018
 */
void turn_counter_Clockwise(oi_t *sensor)
{
    int degrees = 44;
    char display[10];
    while (degrees > 0) //turn left by 45 degrees
    {
        oi_update(sensor);
        /*
         * Checks the bumpers
         */
        if (sensor->bumpLeft && sensor->bumpRight)
        {
            char *conv_disp = "#11#";
            uart_sendStr(conv_disp);
        }
        else if (sensor->bumpLeft)
        {
            char *conv_disp = "#10#";
            uart_sendStr(conv_disp);
        }
        else if (sensor->bumpRight)
        {
            char *conv_disp = "#01#";
            uart_sendStr(conv_disp);
        }

        /*
         * Checks for cliffs
         */
        if (sensor->cliffFrontLeft && sensor->cliffFrontRight)
        {
            char *conv_disp = "!0110!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontLeft)
        {
            char *conv_disp = "!0100!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontRight)
        {
            char *conv_disp = "!0010!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffRight)
        {
            char *conv_disp = "!0001!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffLeft)
        {
            char *conv_disp = "!1000!";
            uart_sendStr(conv_disp);
        }

        /*
         * Check for tape
         */
        if (sensor->cliffFrontLeftSignal > 2750
                && sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0110%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontLeftSignal > 2750)
        {
            char *conv_disp = "%0100%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffFrontRightSignal > 2750)
        {
            char *conv_disp = "%0010%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffRightSignal > 2750)
        {
            char *conv_disp = "%0001%";
            uart_sendStr(conv_disp);
        }
        else if (sensor->cliffLeftSignal > 2750)
        {
            char *conv_disp = "%1000%";
            uart_sendStr(conv_disp);
        }

        /*
         * URGENT: CHECK WHEEL DROP
         */
        if (sensor->wheelDropLeft)
        {
            char *conv_disp = "!!10!!";
            uart_sendStr(conv_disp);
        }
        else if (sensor->wheelDropRight)
        {
            char *conv_disp = "!!01!!";
            uart_sendStr(conv_disp);
        }
        degrees -= sensor->angle;
        sprintf(display,"%s","         ");
        sprintf(display,"%d\r\n", sensor->angle);
        uart_sendStr(display);

        oi_setWheels(100, -100);
    }
    sprintf(display,"%d",degrees);
    uart_sendStr(display);
    oi_setWheels(0, 0); // stop
}

