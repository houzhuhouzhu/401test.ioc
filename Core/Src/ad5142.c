#include "gpio.h"
#include "ad5142.h"

void IIC_Init()
{

    SDA_1;
    SCL_1;
}

void IIC_Star()
{
    SDA_1;
    SCL_1;
    delay_us(4);
    SDA_0;
    delay_us(4);
    SCL_0;
}

void IIC_Stop()
{
    SDA_0;
    SCL_0;
    delay_us(4);
    SDA_1;
    SCL_1;
    delay_us(4);
}
void IIC_Ack()
{
    SCL_0;
    SDA_0;
    delay_us(2);
    SCL_1;
    delay_us(2);
    SCL_0;
}

void IIC_nAck()
{
    SCL_0;
    SDA_1;
    delay_us(2);
    SCL_1;
    delay_us(2);
    SCL_0;
}
void IIC_Send_8bit_Date(uint16_t date)
{
    uint8_t i;
    SCL_0;
    for (i=0;i<8;i++)
    {
        if(date&0x80)
        {
            SDA_1;
        }
        else
        {
            SDA_0;
        }
        date <<=1;
        delay_us(2);
        SCL_1;
        delay_us(2);
        SCL_0;
    }
}
void IIC_Send_16bit_Date(uint16_t date)
{
    uint8_t i;
    SCL_0;
    for(i=0;i<16;i++)
    {
        if(date&0x8000)
        {
            SDA_1;
        }
        else
        {
            SDA_0;
        }
    }
}



void set_r1_value(uint8_t res)   //RES=0X80 MAX
{
    IIC_Star();
    IIC_Send_8bit_Date(0x5E);
    IIC_Ack();
    IIC_Send_8bit_Date(0x10);
    IIC_Ack();
    IIC_Send_8bit_Date(res);
    IIC_Stop();
}
void set_r2_value(uint8_t res)
{
    IIC_Star();
    IIC_Send_8bit_Date(0x5E);
    IIC_Ack();
    IIC_Send_8bit_Date(0x11);
    IIC_Ack();
    IIC_Send_8bit_Date(res);
    IIC_Stop();
}