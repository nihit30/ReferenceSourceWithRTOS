

#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_

#include <stdint.h>

void sendGraphicsLcdCommand(uint8_t command);
void sendGraphicsLcdData(uint8_t data);
void setGraphicsLcdPage(uint8_t page);
void setGraphicsLcdColumn(uint8_t x);
void refreshGraphicsLcd(void);
void clearGraphicsLcd(void);
void initGraphicsLcd(void);
void drawGraphicsLcdPixel(uint8_t x, uint8_t y, uint8_t op);
void drawGraphicsLcdRectangle(uint8_t xul, uint8_t yul, uint8_t dx, uint8_t dy, uint8_t op);
void setGraphicsLcdTextPosition(uint8_t x, uint8_t page);
void putcGraphicsLcd(char c);
void putsGraphicsLcd(char str[]);
void puthGraphicsLcd(uint8_t h);


#endif /* LCDDRIVER_H_ */
