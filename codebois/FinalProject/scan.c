/**
* @file scan.c
* @brief Detects how many objects are in front of the bot and outputs the sensor data of the detected objects to uart
*
* @author Matt Johnson
* @author Thomas Kirby
* @author John Powen
* @date 11/20/2018
*/

#include "openinterface.h"
#include "timer.h"
#include "adc.h"
#include "pulse.h"
#include "servo.h"
#include <math.h>

///Object's data necessary to store
typedef struct object {
    int degrees;
    int start_degree;
    int ping_dist[90];
    int IR_dist[90];
    int width;
    int is_done;
    int is_real;
    int index;
} object_t;


int find_edge(int ping,int ir,int lastIR,int lastPing);

/**
 * This method will be used to initilize the sensors needed to scan for objects
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/20/2018
 */
void scan_init()
{
    servo_init();
    adc_init();
    pulse_init();
}

/**
 * This method will be used to scan in front of the robot when called and output object data to uart
 * @author Matt Johnson
 * @param none
 * @date 11/30/2018
 */
void scan()
{

    scan_init();
    int degree = 0;
    int index = 0;
    int position,k,i,raw,dist;
    int total=0;
    int one = 161;
    double distance=0;
    int lastPing=0;
    int lastIR=0;
    char conv_disp[100];
    object_t object;
    object.is_done = -1;
    object.is_real = 1;
    object.index = 0;
    ///Ticks servo until it has completed a 180 degree turn
    while(degree <= 180 && index < 35)
    {
        position=getCount();
        ///Gets IR sensor data and stores it into distance
        for(k=0;k<50;k++)
        {
            raw = get_adc_data();
            total+=1.0/(0.0000609499*raw+.001829962);
        }
        distance=total/50;
        total=0;
        send_pulse();
        while(getIsDone()==0);
        dist = time2dist();
        ///Checks if edge is detected
        if(find_edge(dist,distance,lastIR,lastPing))
        {
            ///Checks if new object has not been started
            if(object.is_done==-1)
            {
                if(find_edge(dist,distance,lastIR,lastPing)==1)
                {
                    ///Starts new object
                    object.is_done=0;
                    object.start_degree=degree;
                    object.degrees=degree;

                }
            }
            ///checks if currently monitoring object
            else if(object.is_done==0)
            {
                if(find_edge(dist,distance,lastIR,lastPing)==2)
                {

                    ///finishes object
                    object.is_done=-1;
                    object.degrees -=degree;
                }
            }
        }
        if(object.is_done==0)
        {
            ///Sends current object data
            uart_init();
            sprintf(conv_disp,"@%d,%d@\r\n",degree,dist);
            uart_sendStr(conv_disp);
        }
        ///Saves previous data and moves servo
        lastPing=dist;
        lastIR=distance;

        servo_move(position + (2*one));
        degree += 2;
    }
    //reset servo
    servo_move(7500);
}

/**
 * This method will be used to find if the bot is currently looking at the edge of an object
 * @author Matt Johnson
 * @param int ping-current ping data , int ir-current ir data, int lastIR-previous ir data, int lastPing- previous ping data
 * @date 11/30/2018
 */
int find_edge(int ping,int ir,int lastIR,int lastPing)
{
    if(abs(lastIR-ir)>30)
    {

        if(ir<50)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }

    return 0;
}
