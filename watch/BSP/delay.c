#include "delay.h"

/*
 * Microsecond delay based on the DWT (Data Watchpoint and Trace) cycle counter.
 * The counter increments at the core clock (SystemCoreClock = 72 MHz here), so
 * timing is independent of compiler optimisation level - important for the
 * 1-Wire (DS18B20) and I2C bit-bang timing. DelayInit() must be called once
 * before any DelayUs().
 */

void DelayInit(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   /* enable trace subsystem */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;             /* enable cycle counter   */
}

void DelayUs(uint32_t nus)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = nus * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks)
    {
        /* busy wait */
    }
}

void DelayMs(uint32_t nms)
{
    HAL_Delay(nms);
}

void DelayS(uint32_t ns)
{
    while (ns--)
    {
        HAL_Delay(1000);
    }
}
