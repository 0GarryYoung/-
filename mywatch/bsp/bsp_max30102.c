#include "bsp_max30102.h"
#include "bsp_iic_max.h"
#include "bsp_usart1.h"
#include "bsp_delay.h"
#include <stdio.h>

#define max30102_WR_address 0xAE

bool maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
{
    i2c_Start();
    i2c_SendByte(max30102_WR_address | I2C_WR);

    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    i2c_SendByte(uch_addr);
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    i2c_SendByte(uch_data);

    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    i2c_Stop();
    return true;

cmd_fail:
    i2c_Stop();
    return false;
}

bool maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
{
    i2c_Start();
    i2c_SendByte(max30102_WR_address | I2C_WR);

    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    i2c_SendByte((uint8_t)uch_addr);
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    i2c_Start();
    i2c_SendByte(max30102_WR_address | I2C_RD);

    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    {
        *puch_data = i2c_ReadByte();
        i2c_NAck();
    }

    i2c_Stop();
    return true;

cmd_fail:
    i2c_Stop();
    return false;
}

bool maxim_max30102_init(void)
{
    if (!maxim_max30102_write_reg(REG_INTR_ENABLE_1, 0xc0))
        return false;
    if (!maxim_max30102_write_reg(REG_INTR_ENABLE_2, 0x00))
        return false;
    if (!maxim_max30102_write_reg(REG_FIFO_WR_PTR, 0x00))
        return false;
    if (!maxim_max30102_write_reg(REG_OVF_COUNTER, 0x00))
        return false;
    if (!maxim_max30102_write_reg(REG_FIFO_RD_PTR, 0x00))
        return false;
    if (!maxim_max30102_write_reg(REG_FIFO_CONFIG, 0x6f))
        return false;
    if (!maxim_max30102_write_reg(REG_MODE_CONFIG, 0x03))
        return false;
    if (!maxim_max30102_write_reg(REG_SPO2_CONFIG, 0x2F))
        return false;

    if (!maxim_max30102_write_reg(REG_LED1_PA, 0x17))
        return false;
    if (!maxim_max30102_write_reg(REG_LED2_PA, 0x17))
        return false;
    if (!maxim_max30102_write_reg(REG_PILOT_PA, 0x7f))
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

    i2c_Start();
    i2c_SendByte(max30102_WR_address | I2C_WR);

    if (i2c_WaitAck() != 0)
    {
        printf("read fifo failed");
        goto cmd_fail;
    }

    i2c_SendByte((uint8_t)REG_FIFO_DATA);
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    i2c_Start();
    i2c_SendByte(max30102_WR_address | I2C_RD);

    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }

    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 16;
    *pun_red_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 8;
    *pun_red_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    *pun_red_led += un_temp;

    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 16;
    *pun_ir_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 8;
    *pun_ir_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    *pun_ir_led += un_temp;
    *pun_red_led &= 0x03FFFF;
    *pun_ir_led &= 0x03FFFF;

    i2c_Stop();
    return true;
cmd_fail:
    i2c_Stop();
    return false;
}

bool maxim_max30102_reset()
{
    if (!maxim_max30102_write_reg(REG_MODE_CONFIG, 0x40))
        return false;
    else
        return true;
}
