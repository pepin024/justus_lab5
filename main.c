
#include "xc.h"
#include <p24Fxxxx.h>

#include "delayHeader.h"
#include "lcd_display.h"

#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF            // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSECME           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)
// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

const char test[] = {' ','H','E','L','L','O',' ',' ', ' ','W','o','r','l','d','\0'};
char testB[] = "Hello World!";
char testC[] = "^( ^.^ )^  (> ^.^)>  ^( ^.^ )^  <(^.^ <)  v( ^.^ )v  ";

void setup(void)
{
    CLKDIVbits.RCDIV = 0;   //sets clock speed
    AD1PCFG = 0x9fff;       //all pins to digital
    //RB3 = SCL2, RB2 = SDA2
    TRISA = 0x0000;         //sets PORTA to output
    TRISB = 0x0000;         //sets PORTB to output
    I2C2CONbits.I2CEN = 0; //disable I2C bus
    I2C2BRG = 0x9d;      //SCL at 100kHz
    I2C2CONbits.I2CEN = 1; //enable I2C bus
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
}

int main(void) {
    setup();
    lcd_init();
    //lcd_printChar('A');
    lcd_printStrB(testB, 0);
    lcd_printStrB(testC, 1);
    while(1)
    {
        wait(500);
        asm("btg LATA, #0");
        lcd_cmd(0b00011000);
    
    }
    return 0;
}
