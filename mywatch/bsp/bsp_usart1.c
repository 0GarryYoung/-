#include "bsp_usart1.h"
#include <stdio.h>

extern UART_HandleTypeDef huart1;

char Usart1RecBuf[USART1_RXBUFF_SIZE];
uint32_t RxCounter = 0;

#if 1
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
#endif

void uart1_Init(uint32_t bound)
{
    (void)bound;
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void uart1_SendStr(char* SendBuf)
{
    while (*SendBuf)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)SendBuf, 1, HAL_MAX_DELAY);
        SendBuf++;
    }
}

void uart1_send(uint8_t *bufs, uint8_t len)
{
    HAL_UART_Transmit(&huart1, bufs, len, HAL_MAX_DELAY);
}
