#include "bsp_delay.h"

/* DWT 延时 + HAL_Delay */

void DelayInit(void)
{
    /* HAL 已经在 HAL_Init() 中初始化 SysTick */
    /* 使能 DWT 周期计数器，用于微秒级延时 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void DelayMs(uint32_t nms)
{
    HAL_Delay(nms);
}

void DelayUs(uint32_t nus)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = nus * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

void DelayS(uint32_t ns)
{
    for (uint32_t i = 0; i < ns; i++)
    {
        DelayMs(1000);
    }
}

/* 重新配置软件 I2C / 1-Wire / DS1302 引脚为高速 */
void BSP_GPIO_HighSpeed_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* ADXL345 + MAX30102 + DS18B20：开漏输出高速 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = ADXL_SCL_PIN;
    HAL_GPIO_Init(ADXL_SCL_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ADXL_SDA_PIN;
    HAL_GPIO_Init(ADXL_SDA_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MAX_SDA_PIN;
    HAL_GPIO_Init(MAX_SDA_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MAX_SCL_PIN;
    HAL_GPIO_Init(MAX_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);

    /* DS1302：推挽输出高速 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = DS1302_SCK_PIN | DS1302_DAT_PIN | DS1302_RST_PIN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
