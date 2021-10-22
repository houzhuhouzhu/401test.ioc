#ifndef __AD5142_H
#define __AD5142_H
#include "main.h"
#include "delay.h"


#define SDA_0 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define SDA_1 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define SCL_0 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define SCL_1 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)

#define address_w 0x5E
#define address_r 0x5F

void IIC_Init(void);
void IIC_Star(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_nAck(void);
void IIC_Send_16bit_Date(uint16_t date);
void IIC_Send_8bit_Date(uint16_t date);

void ad5142_Init(void);
void set_r1_value(uint8_t res);
void save_r1(uint8_t date);
void set_r2_value(uint8_t res);



#endif