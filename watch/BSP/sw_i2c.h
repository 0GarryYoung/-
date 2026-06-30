#ifndef __SW_I2C_H
#define __SW_I2C_H

#include "stm32f1xx_hal.h"

/*
 * Multi-instance software (bit-bang) I2C.
 *
 * The original Health_watch firmware carried three separate, hard-coded I2C
 * implementations (one hardware I2C for the OLED, two different bit-bang files
 * for ADXL345 and MAX30102). This module replaces all of them with a single
 * handle-based driver: every bus is described by a SW_I2C_Handle_t holding its
 * own SCL/SDA port+pin, so any number of independent software I2C buses can
 * coexist on the same MCU.
 *
 * Pins are driven open-drain and rely on the external pull-up resistors that
 * the OLED / sensor breakout modules provide (the same assumption the original
 * code made). SDA is read back through the input data register while the pin is
 * an open-drain output, so no input/output mode switching is needed.
 */
typedef struct
{
    GPIO_TypeDef *scl_port;
    uint16_t      scl_pin;
    GPIO_TypeDef *sda_port;
    uint16_t      sda_pin;
} SW_I2C_Handle_t;

/* Configure the pins of one bus (enables the GPIO clock, sets open-drain
 * output, releases the bus). Safe to call more than once. */
void    SW_I2C_Init(SW_I2C_Handle_t *h);

void    SW_I2C_Start(SW_I2C_Handle_t *h);
void    SW_I2C_Stop(SW_I2C_Handle_t *h);
uint8_t SW_I2C_WaitAck(SW_I2C_Handle_t *h);          /* 0 = ACK, 1 = no ACK     */
void    SW_I2C_SendAck(SW_I2C_Handle_t *h);
void    SW_I2C_SendNotAck(SW_I2C_Handle_t *h);
void    SW_I2C_SendByte(SW_I2C_Handle_t *h, uint8_t byte);
uint8_t SW_I2C_ReceiveByte(SW_I2C_Handle_t *h);      /* caller sends ACK/NACK   */

/* The three buses on this board (defined in sw_i2c.c). */
extern SW_I2C_Handle_t hi2c_oled;        /* PB6 SCL  / PB7 SDA  -> SSD1306 OLED */
extern SW_I2C_Handle_t hi2c_adxl;        /* PA6 SCL  / PA7 SDA  -> ADXL345      */
extern SW_I2C_Handle_t hi2c_max30102;    /* PB11 SCL / PB10 SDA -> MAX30102     */

#endif /* __SW_I2C_H */
