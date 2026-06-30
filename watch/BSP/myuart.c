#include "myuart.h"

unsigned char rx,rxbuff[255];
unsigned int dex;
void rxcallback()
{
	rxbuff[dex++]=rx;
	HAL_UART_Receive_IT(&huart1,&rx,1);
	if(dex==255)dex=0;
}

