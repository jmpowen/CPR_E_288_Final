/**
* @file servo.c
* @brief Moving Servo using PWM.
*
* @author Matt Johnson
* @author Thomas Kirby
* @author John Powen
* @date 12/15/2018
*/


#include <stdio.h>
#include "openinterface.h"
#include "servo.h"
#include "timer.h"


int count=0;

/**
 * This method will be used to initilize the sensors needed to move the servo
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 12/15/2018
 */
void servo_init()
{

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x700000;

    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_DEN_R |= 0x20;
//
//    GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB5_T1CCP1;


    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    //disable timer
    TIMER1_CTL_R &= ~(TIMER_CTL_TBEN);

    //Set to a 16 bit counter
    TIMER1_CFG_R |= TIMER_CFG_16_BIT;


    //periodic and pwm enable mode
    TIMER1_TBMR_R = (TIMER_TBMR_TBMR_PERIOD|TIMER_TBMR_TBAMS);


    //TIMER1_TBMR_R &= ~(TIMER_TBMR_TBCMR);

    //noninverted
    //TIMER1_CTL_R &= ~(TIMER_CTL_TBPWML);


    //set period
    TIMER1_TBILR_R = (320000 & 0xFFFF);

    TIMER1_TBPR_R = (320000 >> 16) & 0xFF;
    //calibration 04 is 8000
    //calibration 00 is right is 7000, 90 is 22000
    //calibration 12 is right is 8700, 90 is 23500
    //calibration 07 is 7000, 90 is
    //calibration 06 is 7000, 90 is 21500
    servo_move(8500);//7500,90-21500 for cybot 7
    TIMER1_CTL_R |= 0x00000100;

    //servo_move(0);
//
//    //Edge select
//    TIMER1_CTL_R |= TIMER_CTL_TBEVENT_BOTH;
//
//        //Edge-time mode
//    TIMER1_TBMR_R |= TIMER_TBMR_TBCMR;
//
//        //count up.
//    TIMER1_TBMR_R |= TIMER_TBMR_TBCDIR;
//
//        //Set upper bound
//    TIMER1_TBILR_R |= 0xFFFF;
//
//
//    //set timer prescaler
//    TIMER1_TBPR_R = 0;
//
//    //set match prescaler
//    TIMER1_TBPMR_R = 0;
//
//
//    //clears interrupt flags
//    TIMER1_ICR_R |= TIMER_IMR_TBMIM;
//
//    //enable capture input
//    TIMER1_IMR_R |= TIMER_IMR_TBMIM;
//
//    TIMER1_TBPR_R |= (320000 >> 16);
//
//
//    NVIC_EN0_R |= 0x400000;
//
    printf("%x",INT_TIMER1B);
//
//
  //  servo_move(21000);
//

}

/**
 * This method will return current count
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/15/2018
 */
int getCount()
{
    return count;
}

/**
 * This method will be used to move the servo to a desired pulse wdth
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param int pulse width desired to move
 * @date 11/15/2018
 */
void servo_move(unsigned pusle_width)
{
    count=pusle_width;
    TIMER1_TBMATCHR_R = ((320000 - pusle_width) & 0xFFFF);
    TIMER1_TBPMR_R = ((320000 - pusle_width) >> 16);
    timer_waitMillis(100);

}
