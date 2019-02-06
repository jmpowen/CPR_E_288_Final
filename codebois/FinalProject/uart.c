/**
* @file uart.c
* @brief Sending characters and strings to Putty.
*
* @author Sarah Baratta
* @author Chen Ye Lim
* @date 10/7/2018
*/

#include "uart.h"
#include "timer.h"

/**
* Configuration of uart.c
* @author Sarah Baratta
* @author Chen Ye Lim
*
* @date 10/7/2018
*/
void uart_init(void)
{
    ///enable clock to GPIO, R1 = port B
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    ///enable clock to UART1, R1 = UART1. ***Must be done before setting Rx and Tx (See DataSheet)
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;

    ///enable alternate functions on port b pins 0 and 1
    GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1);
    ///enable Rx and Tx on port B on pins 0 and 1
    GPIO_PORTB_PCTL_R |= 0x00000011;
    ///set pin 0 and 1 to digital
    GPIO_PORTB_DEN_R |= (BIT0 | BIT1);
    ///set pin 0 to Rx or input
    GPIO_PORTB_DIR_R &= ~BIT0;
    ///set pin 1 to Tx or output
    GPIO_PORTB_DIR_R |= BIT1;

    uint16_t iBRD = 8;
    uint16_t fBRD = 44;

    ///turn off uart1 while we set it up
    UART1_CTL_R &= ~(UART_CTL_UARTEN);

    ///set baud rate
    UART1_IBRD_R = iBRD;
    UART1_FBRD_R = fBRD;

    ///set frame, 8 data bits, 1 stop bit, no parity, no FIFO
    UART1_LCRH_R = UART_LCRH_WLEN_8 ;
    ///use system clock as source
    UART1_CC_R = UART_CC_CS_SYSCLK;
    ///re-enable enable RX, TX, and uart1
    UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
}

/**
* This method will be used to send char data to the uart
* @author Sarah Baratta
* @author Chen Ye Lim
* @param *data Pointer to send str to uart
* @date 10/7/2018
*/
void uart_sendChar(char data)//Blocking call that sends 1 char over UART 1
{

     ///wait until there is room to send data
     while(UART1_FR_R & 0x20)
    {
    }
     ///send data
     UART1_DR_R = data;
    }

/**
* This method will be used to receive data at the uart
* @author Sarah Baratta
* @author Chen Ye Lim
*
* @date 10/7/2018
*/
int uart_receive(void)
{

    char data = 0;
     ///wait to receive
     while(UART1_FR_R & UART_FR_RXFE)
    {
    }
    ///mask the 4 error bits and grab only 8 data bits
    data = (char)(UART1_DR_R & 0xFF);

    return data;
}

/**
* This method will be used to send string data to the uart
* @author Sarah Baratta
* @author Chen Ye Lim
* @param *data Pointer to send str to uart
* @date 10/7/2018
*/
void uart_sendStr(const char *data)
{
    //TODO for reference see lcd_puts from lcd.c file
    while(*data != '\0')
        {
            uart_sendChar(*data);
            data++;
        }
}
