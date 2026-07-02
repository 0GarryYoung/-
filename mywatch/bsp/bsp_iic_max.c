#include "bsp_iic_max.h"
#include "bsp_delay.h"

#define I2C_SCL_1()  MAX_SCL_H()
#define I2C_SCL_0()  MAX_SCL_L()
#define I2C_SDA_1()  MAX_SDA_H()
#define I2C_SDA_0()  MAX_SDA_L()
#define I2C_SDA_READ()  MAX_SDA_READ()
#define I2C_SCL_READ()  MAX_SCL_READ()

static void i2c_Delay(void)
{
    uint8_t i;
    for (i = 0; i < 30; i++);
}

void bsp_InitI2C(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = MAX_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MAX_SDA_PIN;
    HAL_GPIO_Init(MAX_SDA_PORT, &GPIO_InitStruct);

    i2c_Stop();
}

void i2c_Start(void)
{
    I2C_SDA_1();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SDA_0();
    i2c_Delay();
    I2C_SCL_0();
    i2c_Delay();
}

void i2c_Stop(void)
{
    I2C_SDA_0();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SDA_1();
    i2c_Delay();
}

void i2c_SendByte(uint8_t _ucByte)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            I2C_SDA_1();
        }
        else
        {
            I2C_SDA_0();
        }
        i2c_Delay();
        I2C_SCL_1();
        i2c_Delay();
        I2C_SCL_0();
        if (i == 7)
        {
            I2C_SDA_1();
        }
        _ucByte <<= 1;
        i2c_Delay();
    }
}

uint8_t i2c_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        I2C_SCL_1();
        i2c_Delay();
        if (I2C_SDA_READ())
        {
            value++;
        }
        I2C_SCL_0();
        i2c_Delay();
    }
    return value;
}

uint8_t i2c_WaitAck(void)
{
    uint8_t re;

    I2C_SDA_1();
    i2c_Delay();
    I2C_SCL_1();
    i2c_Delay();
    if (I2C_SDA_READ())
    {
        re = 1;
    }
    else
    {
        re = 0;
    }
    I2C_SCL_0();
    i2c_Delay();
    return re;
}

void i2c_Ack(void)
{
    I2C_SDA_0();
    i2c_Delay();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SCL_0();
    i2c_Delay();
    I2C_SDA_1();
}

void i2c_NAck(void)
{
    I2C_SDA_1();
    i2c_Delay();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SCL_0();
    i2c_Delay();
}

uint8_t i2c_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;

    if (I2C_SDA_READ() && I2C_SCL_READ())
    {
        i2c_Start();
        i2c_SendByte(_Address | I2C_WR);
        ucAck = i2c_WaitAck();
        i2c_Stop();
        return ucAck;
    }
    return 1;
}
