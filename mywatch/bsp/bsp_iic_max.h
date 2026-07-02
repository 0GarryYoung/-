#ifndef __BSP_IIC_MAX_H
#define __BSP_IIC_MAX_H

#include "bsp_config.h"

#define I2C_WR  0
#define I2C_RD  1

void bsp_InitI2C(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

#endif
