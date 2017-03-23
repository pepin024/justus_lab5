/* 
 * File:   lcd_display.h
 * Author: pepin024
 *
 * Created on March 20, 2017, 1:09 PM
 */

#ifndef LCD_DISPLAY_H
#define	LCD_DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

    void lcd_cmdSeqStart(int RS, char command);
    void lcd_cmdSeqMid(int RS, char command);
    void lcd_cmdSeqEnd(int RS, char command);
    void lcd_printStrB(char text[], int row);
    
    void lcd_setCursor(unsigned char x, unsigned char y);
    void lcd_init(void);
    void lcd_cmd(char command);
    void lcd_printChar(char myChar);
    void lcd_printStr(const char *s);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_DISPLAY_H */

