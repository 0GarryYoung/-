#include "bsp_iic_adxl.h"
#include "bsp_delay.h"

void ADXL_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ADXL_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ADXL_SDA_PORT, &GPIO_InitStruct);
}

void ADXL_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ADXL_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ADXL_SDA_PORT, &GPIO_InitStruct);
}

void IIC_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ADXL_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ADXL_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ADXL_SDA_PIN;
    HAL_GPIO_Init(ADXL_SDA_PORT, &GPIO_InitStruct);

    ADXL_SCL_H();
    ADXL_SDA_H();
}

void IIC_start(void)
{
    ADXL_SDA_OUT();
    ADXL_SDA_H();
    ADXL_SCL_H();
    DelayUs(5);
    ADXL_SDA_L();
    DelayUs(5);
    ADXL_SCL_L();
}

void IIC_stop(void)
{
    ADXL_SDA_OUT();
    ADXL_SCL_L();
    ADXL_SDA_L();
    DelayUs(5);
    ADXL_SCL_H();
    ADXL_SDA_H();
    DelayUs(5);
}

void IIC_ack(void)
{
    ADXL_SCL_L();
    ADXL_SDA_OUT();
    ADXL_SDA_L();
    DelayUs(2);
    ADXL_SCL_H();
    DelayUs(5);
    ADXL_SCL_L();
}

void IIC_noack(void)
{
    ADXL_SCL_L();
    ADXL_SDA_OUT();
    ADXL_SDA_H();
    DelayUs(2);
    ADXL_SCL_H();
    DelayUs(2);
    ADXL_SCL_L();
}

u8 IIC_wait_ack(void)
{
    u8 tempTime = 0;
    ADXL_SDA_IN();
    ADXL_SDA_H();
    DelayUs(1);
    ADXL_SCL_H();
    DelayUs(1);

    while (ADXL_SDA_READ())
    {
        tempTime++;
        if (tempTime > 250)
        {
            IIC_stop();
            return 1;
        }
    }

    ADXL_SCL_L();
    return 0;
}

void IIC_send_byte(u8 txd)
{
    u8 i = 0;
    ADXL_SDA_OUT();
    ADXL_SCL_L();
    for (i = 0; i < 8; i++)
    {
        if (txd & 0x80)
            ADXL_SDA_H();
        else
            ADXL_SDA_L();
        txd <<= 1;
        ADXL_SCL_H();
        DelayUs(2);
        ADXL_SCL_L();
        DelayUs(2);
    }
}

u8 IIC_read_byte(u8 ack)
{
    u8 i = 0, receive = 0;
    ADXL_SDA_IN();
    for (i = 0; i < 8; i++)
    {
        ADXL_SCL_L();
        DelayUs(2);
        ADXL_SCL_H();
        receive <<= 1;
        if (ADXL_SDA_READ())
            receive++;
        DelayUs(1);
    }

    if (!ack)
        IIC_noack();
    else
        IIC_ack();

    return receive;
}
