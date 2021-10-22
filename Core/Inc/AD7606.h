#ifndef __AD7606_H_
#define __AD7606_H_

// π”√”≤º˛SPI
#define __STM32_HARDWARE_SPI_

void AD7606_Init(void);
void AD7606_Reset(void);
void AD7606_StartConv(void);
void AD7606_ReadConvData(void);
#endif
