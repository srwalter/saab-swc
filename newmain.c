/*
 * File:   newmain.c
 * Author: srwalter
 *
 * Created on May 12, 2018, 7:51 AM
 */

// PIC16F913 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT/T1OSO pin, I/O function on RA7/OSC1/CLKIN/T1OSI)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#include <xc.h>

#define _XTAL_FREQ 4000000

static idle_counter;

void wait_idle(void) {
#asm
    BANKSEL     (PORTA)
restart:
    MOVLW       0xff
    MOVWF       _idle_counter
loop:
    BTFSS       PORTA,1
    GOTO        restart
    DECF        _idle_counter
    BTFSS       STATUS,2
    GOTO        loop
    NOP
#endasm
}

char read_byte(void) {
    unsigned char data = 0;
    
    // Wait for roughly 9 idle times for sync
    wait_idle();
    PORTBbits.RB1 ^= 1;
    
    // Wait for start bit
    while (RA1 == 1);
    PORTBbits.RB1 ^= 1;
    
    // Delay for 1/3 bit time (1/2 is ideal but assume there's some delay on the start bit)
    // 9600 baud = 104uS bit time; so 35uS
    __delay_us(35);
    // Now we're in the middle of the start bit, so delay a full bit time to get the first data bit
    __delay_us(100);
    if (RA1)
        data |= 1;
    // Bit 2
    __delay_us(100);
    if (RA1)
        data |= 2;
    // Bit 3
    __delay_us(100);
    if (RA1)
        data |= 4;
    // Bit 4
    __delay_us(100);
    if (RA1)
        data |= 8;
    // Bit 5
    __delay_us(100);
    if (RA1)
        data |= 16;
    // Bit 6
    __delay_us(100);
    if (RA1)
        data |= 32;
    // Bit 7
    __delay_us(100);
    if (RA1)
        data |= 64;
    // Bit 8
    __delay_us(100);
    if (RA1)
        data |= 128;    
    
    return data;
}

void main(void) {
    unsigned char data;
    
    // AN1/RA1 to digital input
    ANSELbits.ANS1 = 0;
    // Comparators off, CxIN pins to digital I/O
    CMCON0 = 7;
    // Disable LCD inputs, PORTC to digital I/O
    LCDCON = 0;
    PORTC = 0;
    // Tristate PORTC (infinite resistance))
    TRISC = 0xff;
    
    // RB1 as output
    PORTBbits.RB1 = 1;
    TRISBbits.TRISB1 = 0;
    
    // RC0 = 80k
    // RC1 = 2k
    // RC2 = 2.8k
    // RC3 = RING        
    // RC4 = 8.9k
    // RC5 = 11.8k
    // RC6 = 16k
    // RC7 = 24k
    
    while (1) {
        data = read_byte();
        switch (data) {
            case 1:
                // Answer
                // 2.8k + Ring to ground
                TRISC = ~(1 << 2) & ~(1 << 3);
                break;
                
            case 2:
                // VolUp
                TRISC = ~(1 << 6);
                break;
                
            case 3:
                // VolDown
                TRISC = ~(1 << 7);
                break;
                
            case 5:
                // SeekUp
                TRISC = ~(1 << 4);
                break;
                
            case 6:
                // SeekDown
                TRISC = ~(1 << 5);
                break;
                
            default:
                // No keys, disconnect from ground (i.e. go output)
                TRISC = 0xff;
        }
        
        //PORTC = data;
    }
}
