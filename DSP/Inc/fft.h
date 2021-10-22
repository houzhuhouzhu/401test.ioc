#ifndef __FFT_H
#define __FFT_H

#include "main.h"
#include "arm_math.h"

#define FFT_LENTH 1024 //FFT计算数值
#define Fs 1024000     //ADC采样频率

extern arm_cfft_radix4_instance_f32 scfft;  //设置FFT参数的结构体变量（基4）

extern float Input[FFT_LENTH*2];//前一个数表示实部，后一个表示虚部
extern float Amp[FFT_LENTH];//存放幅值
extern float Fre[FFT_LENTH];//存放频率
extern float Phase[FFT_LENTH];//存放相位
extern float Cmp[FFT_LENTH];//存放模值

void FFTx4_Init(uint16_t fft_lenth,uint8_t ifftFlag,uint8_t bitReverseFlag);//FFT初始化
void FFTx4_Result(float *pScr);//计算FFT的结果
void FFTx4_Cmp(float * pSrc,float * pDst,uint16_t fft_lenth);//计算FFT的模值
void all_result_x4(float * pSrc,float *pDst,float * Amp,float * Fre,float * Phase,uint32_t FREs,uint16_t fft_lenth);
#endif

