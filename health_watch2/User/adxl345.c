/***********************
���ٶȴ�����ADXL345���ݶ�ȡ����
IIC���߽ӿ�
SDA��PD5 -> PB0 (SDA)
SCL��PD6 -> PA7 (SCL)
************************/
#include "adxl345.h"
#include "iic.h"
#include "delay.h"
//��������ʼ��
void adxl345_init()
{
		adxl345_write_reg(0X31,0X0B);		//�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���� 
		adxl345_write_reg(0x2C,0x0B);		//��������ٶ�Ϊ100Hz
		adxl345_write_reg(0x2D,0x08);	   	//����ʹ��,����ģʽ,ʡ������
		adxl345_write_reg(0X2E,0x80);		//��ʹ���ж�		 
	 	adxl345_write_reg(0X1E,0x00);
		adxl345_write_reg(0X1F,0x00);
		adxl345_write_reg(0X20,0x05);	
}
//д�Ĵ�������
void adxl345_write_reg(u8 addr,u8 val) 
{
	IIC_start();  				 
	IIC_send_byte(slaveaddress);     	//����д����ָ��	 
	IIC_wait_ack();	   
    IIC_send_byte(addr);   			//���ͼĴ�����ַ
	IIC_wait_ack(); 	 										  		   
	IIC_send_byte(val);     		//����ֵ					   
	IIC_wait_ack();  		    	   
    IIC_stop();						//����һ��ֹͣ���� 	   
}
//���Ĵ�������
u8 adxl345_read_reg(u8 addr)
{
	u8 temp=0;		 
	IIC_start();  				 
	IIC_send_byte(slaveaddress);	//����д����ָ��	 
	temp=IIC_wait_ack();	   
    IIC_send_byte(addr);   		//���ͼĴ�����ַ
	temp=IIC_wait_ack(); 	 										  		   
	IIC_start();  	 	   		//��������
	IIC_send_byte(regaddress);	//���Ͷ�����ָ��	 
	temp=IIC_wait_ack();	   
    temp=IIC_read_byte(0);		//��ȡһ���ֽ�,�������ٶ�,����NAK 	    	   
    IIC_stop();					//����һ��ֹͣ���� 	    
	return temp;
}
//��ȡ���ݺ���
void adxl345_read_data(short *x,short *y,short *z)
{
	u8 buf[6];
	u8 i;
	IIC_start();  				 
	IIC_send_byte(slaveaddress);	//����д����ָ��	 
	IIC_wait_ack();	   
    IIC_send_byte(0x32);   		//���ͼĴ�����ַ(���ݻ������ʼ��ַΪ0X32)
	IIC_wait_ack(); 	 										  		   
 
 	IIC_start();  	 	   		//��������
	IIC_send_byte(regaddress);	//���Ͷ�����ָ��
	IIC_wait_ack();
	for(i=0;i<6;i++)
	{
		if(i==5)buf[i]=IIC_read_byte(0);//��ȡһ���ֽ�,�������ٶ�,����NACK  
		else buf[i]=IIC_read_byte(1);	//��ȡһ���ֽ�,������,����ACK 
 	}	        	   
    IIC_stop();					//����һ��ֹͣ����
	*x=(short)(((u16)buf[1]<<8)+buf[0]); 	//�ϳ�����    
	*y=(short)(((u16)buf[3]<<8)+buf[2]); 	    
	*z=(short)(((u16)buf[5]<<8)+buf[4]); 
}
//������ȡ����ȡƽ��ֵ����
//times ȡƽ��ֵ�Ĵ���
void adxl345_read_average(float *x,float *y,float *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//��ȡ������Ϊ0
	{
		for(i=0;i<times;i++)//������ȡtimes��
		{
			adxl345_read_data(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;
			DelayMs(5);
		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
}
//void get_angle(float x_angle,float y_angle,float z_angle)
//{
//	short ax,ay,az;
//	adxl345_read_average(&ax,&ay,&az,10);
//	x_angle=atan(ax/sqrt((az*az+ay*ay)));
//	y_angle=atan(ay/sqrt((ax*ax+az*az)));
//	z_angle=atan(sqrt((ax*ax+ay*ay)/az));
//}
