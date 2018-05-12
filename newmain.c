/*
 * File:   newmain.c
 * Author: srwalter
 *
 * Created on May 12, 2018, 7:51 AM
 */


#include <xc.h>

#define _XTAL_FREQ 4000000

void main(void) {
    unsigned char data = 0;
    
    // AN1/RA1 to digital input
    ANSELbits.ANS1 = 0;
    // Comparators off, CxIN pins to digital I/O
    CMCON0 = 7;
    // Disable LCD inputs, PORTC to digital I/O
    LCDCON = 0;
    PORTC = 0;
    // All PORTC as output
    TRISC = 0;
    
    // Wait for start bit
    while (RA1 == 0);
    
    // Delay for 1/3 bit time (1/2 is ideal but assume there's some delay on the start bit)
    // 9600 baud = 104uS bit time; so 35uS
    __delay_us(35);
    data = (data << 1) | RA1;
    // Bit 2
    __delay_us(35);
    data = (data << 1) | RA1;
    // Bit 3
    __delay_us(35);
    data = (data << 1) | RA1;
    // Bit 4
    __delay_us(35);
    data = (data << 1) | RA1;
    // Bit 5
    __delay_us(35);
    data = (data << 1) | RA1;
    // Bit 6
    __delay_us(35);
    data = (data << 1) | RA1;
    // Bit 7
    __delay_us(35);
    data = (data << 1) | RA1;
    // Bit 8
    __delay_us(35);
    data = (data << 1) | RA1;
    
    // XXX turn on LED
    PORTC = data;
}
