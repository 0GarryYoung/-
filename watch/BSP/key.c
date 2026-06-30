#include "key.h"
#include "delay.h"

//key IO init: PB12..PB15 input pull-up
void KEY_Init(void)
{
    GPIO_InitTypeDef gi = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gi.Pin  = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    gi.Mode = GPIO_MODE_INPUT;
    gi.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &gi);
}

//key scan. mode:0 single trigger, 1 continuous. returns 1..4, 0 = none.
//priority KEY0>KEY1>KEY2>KEY3
u8 KEY_Scan(u8 mode)
{
    static u8 key_up=1;//key release flag
    if(mode)key_up=1;
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
    {
        DelayMs(10);//debounce
        key_up=0;
        if(KEY0==0)return 1;
        else if(KEY1==0)return 2;
        else if(KEY2==0)return 3;
        else if(KEY3==0)return 4;
    }else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1)key_up=1;
    return 0;//no key
}
