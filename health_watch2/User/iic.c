#include "iic.h"
#include "delay.h"
void I2C_SDA_OUT(void)//SDA�����������
{
    GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//SDA�������
	GPIO_Init(GPIOB,&GPIO_InitStructure); 						

}

void I2C_SDA_IN(void)//SDA���뷽������
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//SCL��������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}
//����Ϊģ��IIC���ߺ���
void IIC_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PD�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//PD6����Ϊ�������,SCL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	GPIO_SetBits(GPIOB,GPIO_Pin_0); 
//	I2C_SCL_H;
//	I2C_SDA_H;//������
}
void IIC_start()
{
	I2C_SDA_OUT();
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	DelayUs(5);
	IIC_SDA=0;
	DelayUs(5);
	IIC_SCL=0;
}
void IIC_stop()
{
	I2C_SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=0;
	DelayUs(5);
	IIC_SCL=1; 
	IIC_SDA=1;
	DelayUs(5);
}
//��������һ��Ӧ���ź�
void IIC_ack()
{
	IIC_SCL=0;
	I2C_SDA_OUT();
  IIC_SDA=0;
   DelayUs(2);
   IIC_SCL=1;
   DelayUs(5);
   IIC_SCL=0;	
}
//����������Ӧ���ź�
void IIC_noack()
{
	IIC_SCL=0;
	I2C_SDA_OUT();
   IIC_SDA=1;
   DelayUs(2);
   IIC_SCL=1;
   DelayUs(2);
   IIC_SCL=0;
}
//�ȴ��ӻ�Ӧ���ź�
//����ֵ��1 ����Ӧ��ʧ��
//		  0 ����Ӧ��ɹ�
u8 IIC_wait_ack()
{
	u8 tempTime=0;
	I2C_SDA_IN();
	IIC_SDA=1;
	DelayUs(1);
	IIC_SCL=1;
	DelayUs(1);

	while(READ_SDA)
	{
		tempTime++;
		if(tempTime>250)
		{
			IIC_stop();
			return 1;
		}	 
	}

	IIC_SCL=0;
	return 0;
}
void IIC_send_byte(u8 txd)
{
	u8 i=0;
	I2C_SDA_OUT();
	IIC_SCL=0;;//����ʱ�ӿ�ʼ���ݴ���
	for(i=0;i<8;i++)
	{
		IIC_SDA=(txd&0x80)>>7;//��ȡ�ֽ�
		txd<<=1;
		IIC_SCL=1;
		DelayUs(2); //��������
		IIC_SCL=0;
		DelayUs(2);
	}
}
//��ȡһ���ֽ�
u8 IIC_read_byte(u8 ack)
{
	u8 i=0,receive=0;
	I2C_SDA_IN();
   for(i=0;i<8;i++)
   {
   		IIC_SCL=0;
		DelayUs(2);
		IIC_SCL=1;
		receive<<=1;//����
		if(READ_SDA)
		   receive++;//������ȡ��λ
		DelayUs(1);	
   }

   	if(!ack)
	   	IIC_noack();
	else
		IIC_ack();

	return receive;//���ض�ȡ�����ֽ�
}

	
