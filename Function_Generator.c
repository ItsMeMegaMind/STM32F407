#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_DMA.H>
#include <STM32F4XX_DAC.H>
#include <STM32F4XX_ADC.H>
#include <STM32F4XX_TIM.H>
#include <STM32F4XX_GPIO.H>

#define  Resolution 128

uint32_t Frequency = 2000;
uint16_t Timer_Period;

const uint16_t Function[Resolution] = { 2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897,
        								2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640,
        								3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002,
        								4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894,
        								3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344,
        								3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483,
        								2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517,
        								1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679,
        								610, 545, 484, 426, 373, 323, 278, 237, 201, 169,
        								141, 118, 100, 87, 79, 75, 76, 82, 93, 109,
        								129, 154, 184, 218, 257, 300, 347, 399, 455, 514,
        								577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287,
        								1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 };

void FG_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void FG_Timer_Configure()
{
	Timer_Period = (((84000000)/(Resolution*Frequency)) - 1);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period        		= Timer_Period;
	TIM_TimeBaseInitStructure.TIM_Prescaler     		= 0;
	TIM_TimeBaseInitStructure.TIM_ClockDivision			= 0;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter		= 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode   		= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);

	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	TIM_Cmd(TIM6, ENABLE);
}

void FG_DAC_Configure()
{
	DAC_InitTypeDef DAC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
}


void FG_DMA_Configure()
{
	DMA_InitTypeDef DMA_InitSturture;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Stream5);
	DMA_InitSturture.DMA_Channel            = DMA_Channel_7;
	DMA_InitSturture.DMA_PeripheralBaseAddr = 0x40007408;
	DMA_InitSturture.DMA_Memory0BaseAddr    = (uint32_t)&Function;
	DMA_InitSturture.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
	DMA_InitSturture.DMA_BufferSize         = Resolution;
	DMA_InitSturture.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitSturture.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitSturture.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitSturture.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitSturture.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitSturture.DMA_Priority           = DMA_Priority_High;
	DMA_InitSturture.DMA_FIFOMode           = DMA_FIFOMode_Disable;
	DMA_InitSturture.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
	DMA_InitSturture.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
	DMA_InitSturture.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitSturture);

	DMA_Cmd(DMA1_Stream5, ENABLE);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

int main()
{
	SystemInit();

	FG_GPIO_Configure();
	FG_Timer_Configure();
	FG_DAC_Configure();
	FG_DMA_Configure();

	while(1);

}
