#include "fft.h"

arm_cfft_radix4_instance_f32 scfft;//定义初始化结构体


float Input[FFT_LENTH*2];//前一个数表示实部，后一个表示虚部
float Amp[FFT_LENTH];//存放幅值
float Fre[FFT_LENTH];//存放频率
float Phase[FFT_LENTH];//存放相位
float Cmp[FFT_LENTH];//存放模值


/****************************************************************************
* 名    称: void FFTx4_init(u8 ifftFlag,u8 bitReverseFlag)
* 功    能：FFT初始化函数（基4）
* 入口参数：fft_lenth:FFT的长度，也就是采样点的个数
            ifftFlag：于指定是傅里叶变换(0)还是反傅里叶变换(1)
						bitReverseFlag：是否按位取反
                 
* 返回参数：无
* 说    明：
****************************************************************************/
void FFTx4_Init(uint16_t fft_lenth,uint8_t ifftFlag,uint8_t bitReverseFlag)
{
  arm_cfft_radix4_init_f32(&scfft,fft_lenth,ifftFlag,bitReverseFlag);//初始化scfft结构体，设定FFT相关参数

}

/****************************************************************************
* 名    称: void FFTx4_Result(float * pSrc)
* 功    能：FFT计算函数（基4）
* 入口参数：pSrc：pSrc 传入采集到的输入信
									号数据（实部+虚部形式），同时 FFT 变换后的数据，也按顺序存放在 pSrc 里面                
* 返回参数：无
* 说    明：pSrc的长度必须>=2*length
****************************************************************************/
void FFTx4_Result(float *pScr)
{
  arm_cfft_radix4_f32(&scfft,pScr);//FFT计算
}

/****************************************************************************
* 名    称: void FFTx4_Cmp(float * pSrc,float * pDst)
* 功    能：对FFT结果求模值（基4）
* 入口参数：pSrc：需要求模的复数指针（实部+虚部形式）
					 pDst：存放模值
					

* 返回参数：无
* 说    明：pSrc的长度必须>=2*length
****************************************************************************/
void FFTx4_Cmp(float * pSrc,float * pDst,uint16_t fft_lenth)
{
  arm_cmplx_mag_f32(pSrc,pDst,fft_lenth);
}

/****************************************************************************
* 名    称: void all_result_x4(uint16_t fft_lenth,float * pSrc,float * Amp,float * rate,float * Phase,uint32_t Fs)
* 功    能：计算各点幅值、频率、相位
* 入口参数：pSrc：原始信号的复数指针（实部+虚部形式）
            pDst：存放得到的模值
            Amp：存放幅值
            Fre：存放各点的频率
            Phase：存放各点的相位
            FREs：采样点的个数(采样频率)

* 返回参数：无
* 说    明：pSrc的长度必须>=2*size
****************************************************************************/
void all_result_x4(float * pSrc,float *pDst,float * Amp,float * Fre,float * Phase,uint32_t FREs,uint16_t fft_lenth)
{ 
  uint32_t i;
  
	arm_cfft_radix4_f32(&scfft,pSrc);	//FFT计算（基4）
	arm_cmplx_mag_f32(pSrc,pDst,fft_lenth);	//计算复数模值

	//计算各点幅值、频率、相位
	for(i = 0;i < fft_lenth;i++)
	{
		if(i == 0)	
    {
      Amp[0]=pDst[0]/fft_lenth;	//第一个点直接除以size(z直流分量)
    }
		else
    {
      Amp[i]=pDst[i]*2/fft_lenth;	//其他点除以size/2
    }
		Fre[i] = FREs/fft_lenth*i;//	各点频率
		Phase[i] = atan2(pSrc[2*i+1], pSrc[2*i]); /* atan2求解的结果范围是(-pi, pi], 弧度制 */ 
	}

	
}

