/** \file max30102.c ******************************************************
* Project: MAXREFDES117#
* Description: embedded controller driver for the MAX30102.
* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Ported from the original Health_watch max30102.c: the myiic.c primitives have
* been replaced one-for-one by the handle-based sw_i2c calls (bus hi2c_max30102,
* PB11 SCL / PB10 SDA). i2c_ReadByte->SW_I2C_ReceiveByte, and the explicit
* i2c_Ack/i2c_NAck the callers already issue map to SW_I2C_SendAck/SendNotAck.
*******************************************************************************/

#include "max30102.h"
#include "sw_i2c.h"
#include <stdio.h>

#define BUS  (&hi2c_max30102)

#define I2C_WR  0
#define I2C_RD  1

#define max30102_WR_address 0xAE

bool maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
{
    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, max30102_WR_address | I2C_WR);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }
    SW_I2C_SendByte(BUS, uch_addr);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }
    SW_I2C_SendByte(BUS, uch_data);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }
    SW_I2C_Stop(BUS);
    return true;

cmd_fail:
    SW_I2C_Stop(BUS);
    return false;
}

bool maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
{
    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, max30102_WR_address | I2C_WR);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }
    SW_I2C_SendByte(BUS, (uint8_t)uch_addr);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }

    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, max30102_WR_address | I2C_RD);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }

    *puch_data = SW_I2C_ReceiveByte(BUS);
    SW_I2C_SendNotAck(BUS);     /* NACK after the last byte */
    SW_I2C_Stop(BUS);
    return true;

cmd_fail:
    SW_I2C_Stop(BUS);
    return false;
}

bool maxim_max30102_init(void)
{
    if(!maxim_max30102_write_reg(REG_INTR_ENABLE_1, 0xc0)) // INTR setting
        return false;
    if(!maxim_max30102_write_reg(REG_INTR_ENABLE_2, 0x00))
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_WR_PTR, 0x00)) //FIFO_WR_PTR[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_OVF_COUNTER, 0x00)) //OVF_COUNTER[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_RD_PTR, 0x00)) //FIFO_RD_PTR[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_CONFIG, 0x6f)) //sample avg = 8, fifo rollover=false, fifo almost full = 17
        return false;
    if(!maxim_max30102_write_reg(REG_MODE_CONFIG, 0x03))  //0x02 Red only, 0x03 SpO2, 0x07 multimode
        return false;
    if(!maxim_max30102_write_reg(REG_SPO2_CONFIG, 0x2F)) // ADC range 4096nA, 400 Hz, 411us pulse width
        return false;
    if(!maxim_max30102_write_reg(REG_LED1_PA, 0x17))  //~4.5mA LED1
        return false;
    if(!maxim_max30102_write_reg(REG_LED2_PA, 0x17))  //~4.5mA LED2
        return false;
    if(!maxim_max30102_write_reg(REG_PILOT_PA, 0x7f))  //~25mA pilot LED
        return false;
    return true;
}

bool maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
    uint32_t un_temp;
    uint8_t uch_temp;
    *pun_ir_led = 0;
    *pun_red_led = 0;
    maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
    maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);

    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, max30102_WR_address | I2C_WR);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        printf("read fifo failed");
        goto cmd_fail;
    }
    SW_I2C_SendByte(BUS, (uint8_t)REG_FIFO_DATA);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }

    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, max30102_WR_address | I2C_RD);
    if (SW_I2C_WaitAck(BUS) != 0)
    {
        goto cmd_fail;
    }

    un_temp = SW_I2C_ReceiveByte(BUS);
    SW_I2C_SendAck(BUS);
    un_temp <<= 16;
    *pun_red_led += un_temp;
    un_temp = SW_I2C_ReceiveByte(BUS);
    SW_I2C_SendAck(BUS);
    un_temp <<= 8;
    *pun_red_led += un_temp;
    un_temp = SW_I2C_ReceiveByte(BUS);
    SW_I2C_SendAck(BUS);
    *pun_red_led += un_temp;

    un_temp = SW_I2C_ReceiveByte(BUS);
    SW_I2C_SendAck(BUS);
    un_temp <<= 16;
    *pun_ir_led += un_temp;
    un_temp = SW_I2C_ReceiveByte(BUS);
    SW_I2C_SendAck(BUS);
    un_temp <<= 8;
    *pun_ir_led += un_temp;
    un_temp = SW_I2C_ReceiveByte(BUS);
    SW_I2C_SendAck(BUS);
    *pun_ir_led += un_temp;
    *pun_red_led &= 0x03FFFF; //Mask MSB [23:18]
    *pun_ir_led &= 0x03FFFF; //Mask MSB [23:18]

    SW_I2C_Stop(BUS);
    return true;
cmd_fail:
    SW_I2C_Stop(BUS);
    return false;
}

bool maxim_max30102_reset()
{
    if(!maxim_max30102_write_reg(REG_MODE_CONFIG, 0x40))
        return false;
    else
        return true;
}
