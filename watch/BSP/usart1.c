#include "usart1.h"

/*
 * printf retarget over HAL USART1 (PA9 TX / PA10 RX, 115200 baud - set up by
 * CubeMX's MX_USART1_UART_Init). The __MICROLIB guard makes this build whether
 * or not "Use MicroLIB" is ticked in the Keil target options.
 */
#if !defined(__MICROLIB)
#pragma import(__use_no_semihosting)
struct __FILE { int handle; };
FILE __stdout;
void _sys_exit(int x) { (void)x; }
void _ttywrch(int ch) { (void)ch; }
#endif

int fputc(int ch, FILE *f)
{
    (void)f;
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

void uart1_SendStr(char *SendBuf)
{
    while (*SendBuf)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)SendBuf, 1, 0xFFFF);
        SendBuf++;
    }
}

void uart1_send(unsigned char *bufs, unsigned char len)
{
    HAL_UART_Transmit(&huart1, bufs, len, 0xFFFF);
}
