#ifndef __OLED_H
#define __OLED_H

#include "stm32f1xx_hal.h"

/*
 * 128x64 SSD1306 OLED driver.
 *
 * Functionally identical to the original Health_watch "OLED_I2C.c" (same rich
 * pixel-based API: 6x8 / 8x16 ASCII, 16x16 Chinese, inverse video, BMP), but
 * the transport is the multi-instance software I2C (sw_i2c) running on the
 * dedicated OLED bus (PB6 SCL / PB7 SDA) instead of the STM32 hardware I2C.
 *
 * Coordinates: x = 0..127 (pixel column), y = 0..7 (page).
 * TextSize:    1 = 6x8, 2 = 8x16.   mode: 0 = normal, 1 = inverse.
 */
#define OLED_ADDRESS  0x78

void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char TextSize, uint8_t mode);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N, uint8_t mode);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);

#endif /* __OLED_H */
