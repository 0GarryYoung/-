#ifndef __USART1_H
#define __USART1_H

#include "usart.h"   /* huart1 + HAL UART (configured by CubeMX MX_USART1_UART_Init) */
#include <stdio.h>

void uart1_SendStr(char *SendBuf);
void uart1_send(unsigned char *bufs, unsigned char len);

#endif /* __USART1_H */
