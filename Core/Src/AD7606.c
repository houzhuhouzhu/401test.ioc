//#include "stm32g4xx_hal.h"
//#include "stm32g4xx_hal_rcc.h"
//#include "stm32g4xx_hal_gpio.h"
//#include "stm32g4xx_hal_spi.h"
//#include "stm32g4xx_hal_exti.h"

#include "main.h"
#include "delay.h"

#include "ad7606.h"

#include "stdio.h"

SPI_HandleTypeDef AD7606_SPI;//AD7606_SPI句柄

extern UART_HandleTypeDef lpuart1;

extern int16_t AD7606_ADCValue[8];

/*io连接
SPI3

SCK RD PA5
MISO PA6
CS PA15 B2
BUSY PD2(外部中断) B6
RST PA14  A12
CONV PA13 A11
*/

void AD7606_Init(void)
{
	#ifdef __STM32_HARDWARE_SPI_
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();
	
	AD7606_SPI.Instance = SPI1;
	AD7606_SPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	AD7606_SPI.Init.CLKPhase = SPI_PHASE_1EDGE;
	AD7606_SPI.Init.CLKPolarity = SPI_POLARITY_HIGH;
	AD7606_SPI.Init.DataSize = SPI_DATASIZE_16BIT;
	AD7606_SPI.Init.Direction = SPI_DIRECTION_2LINES;
	AD7606_SPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
	AD7606_SPI.Init.Mode = SPI_MODE_MASTER;
	AD7606_SPI.Init.NSS = SPI_NSS_SOFT;
	AD7606_SPI.Init.TIMode = SPI_TIMODE_DISABLE;

	GPIO_InitTypeDef AD7606_GPIO;
	
	AD7606_GPIO.Pin = GPIO_PIN_11 | GPIO_PIN_12; //三个普通IO
	AD7606_GPIO.Mode = GPIO_MODE_OUTPUT_PP;
	AD7606_GPIO.Pull = GPIO_PULLUP;
	AD7606_GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &AD7606_GPIO);

    AD7606_GPIO.Pin = GPIO_PIN_2; //三个普通IO
    AD7606_GPIO.Mode = GPIO_MODE_OUTPUT_PP;
    AD7606_GPIO.Pull = GPIO_PULLUP;
    AD7606_GPIO.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &AD7606_GPIO);
	
	AD7606_GPIO.Pin = GPIO_PIN_5 | GPIO_PIN_6;//|SPI IO
	AD7606_GPIO.Mode = GPIO_MODE_AF_PP;
	AD7606_GPIO.Pull = GPIO_NOPULL;
	AD7606_GPIO.Speed = GPIO_SPEED_FREQ_HIGH;
	AD7606_GPIO.Alternate = GPIO_AF5_SPI1;
	
	HAL_GPIO_Init(GPIOA, &AD7606_GPIO);
	
	HAL_SPI_Init(&AD7606_SPI);
	__HAL_SPI_ENABLE(&AD7606_SPI);
	
	uint8_t c=0xF1;
	
	HAL_SPI_Transmit(&AD7606_SPI,&c,1,0xFFFF);
	
	AD7606_GPIO.Pin = GPIO_PIN_6;//忙信号（外部中断）
	AD7606_GPIO.Mode=GPIO_MODE_IT_FALLING;
	AD7606_GPIO.Pull = GPIO_NOPULL;
	AD7606_GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &AD7606_GPIO);
	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn  ,0,0);
	
/*IO组和IO号定义*/
/*IO组*/
#define AD7606_CS_GPIOG		GPIOB
#define AD7606_RST_GPIOG	GPIOA
#define AD7606_CONV_GPIOG	GPIOA
	
#define AD7606_CS_GPION		GPIO_PIN_2
#define AD7606_RST_GPION	GPIO_PIN_12
#define AD7606_CONV_GPION	GPIO_PIN_11
	
#endif
	
	AD7606_Reset();
	
}

void AD7606_Reset(void)
{
	HAL_GPIO_WritePin(AD7606_RST_GPIOG, AD7606_RST_GPION, GPIO_PIN_RESET);
	delay_us(10);
	HAL_GPIO_WritePin(AD7606_RST_GPIOG, AD7606_RST_GPION, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(AD7606_RST_GPIOG, AD7606_RST_GPION, GPIO_PIN_RESET);
	delay_us(10);
}

void AD7606_StartConv(void)
{
	AD7606_Reset();
	HAL_GPIO_WritePin(AD7606_CS_GPIOG, AD7606_CS_GPION, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD7606_CONV_GPIOG, AD7606_CONV_GPION, GPIO_PIN_SET);
	delay_us(100);
	
	HAL_GPIO_WritePin(AD7606_CONV_GPIOG, AD7606_CONV_GPION, GPIO_PIN_RESET);//转换
	delay_us(100);
	HAL_GPIO_WritePin(AD7606_CONV_GPIOG, AD7606_CONV_GPION, GPIO_PIN_SET);
	
	//等待BUSY结束
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

//处理BUSY忙信号
void EXTI2_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2); 
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_2)AD7606_ReadConvData();
}

	uint8_t AD7606_ConvDataBuf[16];

  void AD7606_ReadConvData(void)
{
	HAL_GPIO_WritePin(AD7606_CS_GPIOG, AD7606_CS_GPION, GPIO_PIN_RESET);//拉低片选
	delay_us(100);
	
	//读

	HAL_SPI_Receive(&AD7606_SPI,(uint8_t*)AD7606_ADCValue,16,0xFFFF);
	
	HAL_GPIO_WritePin(AD7606_CS_GPIOG, AD7606_CS_GPION, GPIO_PIN_SET);
	
	char value[20];
	sprintf(value,"CH%d=%5d\r\n",1,AD7606_ADCValue[0]);
    //HAL_UART_Transmit(&lpuart1,(uint8_t*)value,20,0xFFFF);
	
//	//解码
//	for(uint8_t i=0;i<8;i++)
//	{
//		AD7606_ADCValue[i] = (AD7606_ConvDataBuf[i*2]<<8)+AD7606_ConvDataBuf[i*2+1];
//	}
//	char value[20];
//	sprintf(value,"CH%d=%5d\n",1,AD7606_ADCValue[1]);
//  HAL_UART_Transmit(&lpuart1,(uint8_t*)value,20,0xFFFF);
}

//void AD7606_SendConvValue(UART_HandleTypeDef *huart)
//{
//	
//}

