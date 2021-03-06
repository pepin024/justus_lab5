/*
 * File:   lcd_display.c
 * Author: Justus
 *
 * Created on March 18, 2017, 8:07 PM
 */
#include "xc.h"
#include "delayHeader.h"
#include <p24Fxxxx.h>

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

#define SLAVE_ADDRESS 0b01111100 //set to write, last bit is R/nW

unsigned char contrast = 0b00010111; //global variable for constrast
const char test[] = "Do This   Work";

void wait(int t)
{
    int i = 0;
    while(i<t)
    {
        delay_1ms();
        i++;
    }
}

void lcd_cmd(char command)
{
    I2C2CONbits.SEN = 1; //START bit
    while(I2C2CONbits.SEN==1);//wait for SEN to clear
     IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = SLAVE_ADDRESS; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = 0b00000000;//control byte
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = command; //data byte
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2CONbits.PEN = 1; //STOP bit
    while(I2C2CONbits.PEN == 1);//wait for Stop bit to complete
     IFS3bits.MI2C2IF = 0; //reset
}

void lcd_init(void)
{
    wait(50);
    lcd_cmd(0b00111000);//function set, normal instruction mode <--- WHAT IS THE POINT
    lcd_cmd(0b00111001);//function set, extended instruction mode
    lcd_cmd(0b00010100);//internal OSC frequency
    
    unsigned char contrastmanip = contrast &0b00001111; //lower 4 bits
    contrastmanip |= 0b01110000; //results in 0111<C3:C0>
    lcd_cmd(contrastmanip); // Contrast Set
    
    contrastmanip = contrast >> 4; //sets bits C4 and C5 at the end
    contrastmanip &= 0b00000011;
    contrastmanip |= 0b01010100; //results in 010101<C5:C4>
    lcd_cmd(contrastmanip); // Power/ICON/Contrast control
    
    lcd_cmd(0b01101100); //Follower control
    wait(200);
    lcd_cmd(0b00111000); //function set, normal instruction mode
    lcd_cmd(0b00001100); //Display on, cursor off
    lcd_cmd(0b00000001); //clears display
    wait(2);
}

void lcd_setCursor(unsigned char x, unsigned char y)
{//sets cursor to row x column y, x must be 0 or 1, y must be 0 through 7
    unsigned char package = 0b01000000; // left shift by 6
    package &= x<<6;
    y &= 0b00001111;
    package |= y;
    package |= 0b10000000; // after all these operations package is: 1<row 0:1>00 <col 0:7>
    lcd_cmd(package);
}

void lcd_printChar(char myChar)
{
    I2C2CONbits.SEN = 1; //START bit
    while(I2C2CONbits.SEN==1);//wait for SEN to clear
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = SLAVE_ADDRESS; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = 0b01000000;//control byte, RS =1
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = myChar; //data byte
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2CONbits.PEN = 1; //STOP bit
    while(I2C2CONbits.PEN == 1);//wait for Stop bit to complete
    IFS3bits.MI2C2IF = 0; //reset
}

void lcd_printStr(const char *s)
{
    int i = 0;
    char out;
    
    I2C2CONbits.SEN = 1; //START bit
    while(I2C2CONbits.SEN==1);//wait for SEN to clear
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = SLAVE_ADDRESS; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    
    while(i<16)
    {
        if(i==8)//change to check if s is 12, 12 is newline
        {
            lcd_setCursor(1,0);
            I2C2CONbits.SEN = 1; //START bit
            while(I2C2CONbits.SEN==1);//wait for SEN to clear
            IFS3bits.MI2C2IF = 0; //reset
    
            I2C2TRN = SLAVE_ADDRESS; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
             while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
             IFS3bits.MI2C2IF = 0; //reset
    
            
        }
        out = *s;
        s++;
        if(*s == 0 || i==15 || i==7)
        {
            I2C2TRN = 0b01000000;//control byte, RS =1, last char
            while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
            IFS3bits.MI2C2IF = 0; //reset
            
            I2C2TRN = out; //data byte
            while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
            IFS3bits.MI2C2IF = 0; //reset
            
            I2C2CONbits.PEN = 1; //STOP bit
            while(I2C2CONbits.PEN == 1);//wait for Stop bit to complete
            IFS3bits.MI2C2IF = 0; //reset
            
            if(*s == 0 || i==15)
            {
            break;
            }
        }
        else
        {
            I2C2TRN = 0b11000000;//control byte, RS =1, last char
            while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
            IFS3bits.MI2C2IF = 0; //reset
            
            I2C2TRN = out; //data byte
            while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
            IFS3bits.MI2C2IF = 0; //reset
            
            
        }
        i++;
    }
}

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
    lcd_printStr(&test);
    wait(500);
    lcd_cmd(0b00011000); //shifts display
    wait(500);
    lcd_cmd(0b00011000); //shifts display
    wait(500);
    lcd_cmd(0b00011000); //shifts display
    wait(500);
    lcd_cmd(0b00011000); //shifts display
    wait(500);
    lcd_cmd(0b00011000); //shifts display
    wait(500);
    lcd_cmd(0b00011000); //shifts display
    wait(500);
    lcd_cmd(0b00011000); //shifts display
    while(1)
    {
        wait(500);
        asm("btg LATA, #0");
    
    }
    return 0;
}
