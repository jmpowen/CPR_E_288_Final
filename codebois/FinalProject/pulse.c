/**
* @file pulse.c
* @brief Creating a pulse using the PWM.
*
* @author Matt Johnson
* @author Thomas Kirby
* @author John Powen
* @date 10/10/2018
*/

#include "openinterface.h"
#include "lcd.h"
#include "timer.h"
#include "adc.h"
#include "pulse.h"
#include <limits.h>

volatile enum {LOW, HIGH, DONE} state;
volatile unsigned rising_time; // start time of the return pulse
volatile unsigned falling_time; // end time of the return pulse
int over=0;

/**
 * This method will be used as the exception handler for the ping sensor
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/10/2018
 */
void TIMER3B_Handler(void)
{

    if(TIMER3_RIS_R &= 0x400) {

        if(state == LOW) {
            TIMER3_ICR_R=TIMER_ICR_CBECINT;
            rising_time = TIMER3_TBR_R;
            state = HIGH;
        } else if(state == HIGH) {
            TIMER3_ICR_R=TIMER_ICR_CBECINT;
            falling_time = TIMER3_TBR_R;
            state = DONE;

            TIMER3_CTL_R&=~TIMER_CTL_TBEN;
        }
    }
    TIMER3_ICR_R |= 0x400;
}

/**
 * This method will be used to initialize the needed sensor for the servo to move
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/10/2018
 */
int pulse_init()
{
//    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;
//    SYSCTL_RCGCGPIO_R  |= 2;
//
//    GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB3_T3CCP1;//port control pb3 to t3ccp1
//    GPIO_PORTB_DEN_R |= 8;
//
//
//    SYSCTL_RCGTIMER_R |;//enable the clock for timer3
    //disable timer3
    //GPTMTBMR edge time and count up and capture mode
    //GPTMBILR sets max value for timer set to FFFFFFFFF
    //GPTMIMR Mask capture mode interrupt mask for timer 3
    /*
     * //   NVIC_ENn_R = 0x????????;
    //go to page 105 and find the corresponding interrupt numbers for TIMER4 A&B
    //then set those bits in the correct interrupt set EN register (p. 142)
    //INT_TIMER4A = 70;
    //INT_TIMER4B = 71;
    NVIC_EN2_R=0b11000000;
    IntRegister(INT_TIMER4A, TIMER4A_Handler); //register TIMER4A interrupt handler
    //NVIC_EN2_R==0b10000000;
    IntRegister(INT_TIMER4B, TIMER4B_Handler); //register TIMER4B interrupt handler

    IntMasterEnable(); //Initialize global interrupts
     */
    //GPTMCTL GPTM TIMER B event both edges, also enables


        SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;

        SYSCTL_RCGCGPIO_R |= 0x02;

        GPIO_PORTB_DEN_R |= 0x08;
        GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB3_T3CCP1;

        TIMER3_CTL_R &= ~TIMER_CTL_TBEN;

        //Edge select
        TIMER3_CTL_R |= TIMER_CTL_TBEVENT_BOTH;

        //Set to a 16 bit counter
        TIMER3_CFG_R |= TIMER_CFG_16_BIT;

        //Capture mode
        TIMER3_TBMR_R |= TIMER_TBMR_TBMR_CAP;

        //Edge-time mode
        TIMER3_TBMR_R |= TIMER_TBMR_TBCMR;

        //count up.
        TIMER3_TBMR_R |= TIMER_TBMR_TBCDIR;

        //Set upper bound
        TIMER3_TBILR_R |= 0xFFFF;

        //enable capture input
        TIMER3_IMR_R |= TIMER_IMR_CBEIM;

        //clears interrupt flags
        TIMER3_ICR_R |= TIMER_ICR_CBECINT;

        //105 and 142 interrupt #
        //Timer 3B: 36 Register 5, set Enable (EN1) BIT4 or 0x10
        NVIC_EN1_R |= 0x10;

        //register TIMER3B interrupt handler
        IntRegister(INT_TIMER3B, TIMER3B_Handler);

        //initialize global interrupts
        IntMasterEnable();
    return 1;
}

/**
 * This method will be used as the exception handler for the ping sensor
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/10/2018
 */
void send_pulse()
{
    state = LOW;
 GPIO_PORTB_AFSEL_R &= 0xFFF0;
 GPIO_PORTB_DIR_R |= 0x08; // set PB3 as output
 GPIO_PORTB_DATA_R |= 0x08; // set PB3 to high
 // wait at least 5 microseconds based on data sheet
 timer_waitMicros(6);
 GPIO_PORTB_DATA_R &= 0x7; // set PB3 to low
 GPIO_PORTB_DIR_R &= 0x7; // set PB3 as input
 GPIO_PORTB_AFSEL_R |= 0x8;
 TIMER3_CTL_R|=TIMER_CTL_TBEN;

}

/**
 * This method will be used to convert time in clock counts to single-trip distance in mm
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/10/2018
 */
int time2dist()
{
    int cycles;
    double time;
    if(rising_time>falling_time)
    {
        over++;
        cycles=falling_time+((1<<24)-rising_time);
        time = (double)cycles/16000000.0;
        return (int)(time*17000);
    }
    cycles =  falling_time-rising_time;
    time = (double)cycles/16000000.0;
    return (int)(time*17000);
    //unsigned temp = 0;
    //temp--;

}

/**
 * This method will be used to see if pulse is done sending
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/10/2018
 */
int getIsDone()
{
    if(state==DONE)
    {
        return 1;
    }
    return 0;
}

/**
 * This method will be used to return the over value
 * @author Matt Johnson
 * @author Thomas Kirby
 * @author John Powen
 * @param none
 * @date 11/10/2018
 */
int getOver()
{
    return over;
}
