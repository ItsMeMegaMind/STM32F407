// Incomplete Project File
// Only for checking audio buffer exchange at half DMA transfer to internal DAC
// Incomplete Project File

#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_DMA.H>
#include <STM32F4XX_DAC.H>
#include <STM32F4XX_ADC.H>
#include <STM32F4XX_TIM.H>
#include <STM32F4XX_GPIO.H>
#include <MISC.H>


#define  Resolution 64000

uint32_t Frequency = 48000;
uint16_t Timer_Period;
uint32_t i=0,l=32000,k=1;

uint8_t Audio_Wav_Buffer_0[Resolution];
uint8_t Audio_Sample[Resolution];

void Audio_Codec_DAC_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
}

void Audio_Codec_DAC_Timer_Configure()
{
	Timer_Period = (84000000/Frequency) - 1;

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

void Audio_Codec_DAC_DAC_Configure()
{
	DAC_InitTypeDef DAC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
}


void Audio_Codec_DAC_DMA_Configure()
{
	DMA_InitTypeDef 	DMA_InitSturture;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Stream5);
	DMA_InitSturture.DMA_Channel            = DMA_Channel_7;
	DMA_InitSturture.DMA_PeripheralBaseAddr = 0x40007408;
	DMA_InitSturture.DMA_Memory0BaseAddr    = (uint8_t)&Audio_Sample;
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

	//DMA_ITConfig(DMA1_Stream5, DMA_IT_HT, ENABLE);

	DMA_Cmd(DMA1_Stream5, ENABLE);

	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void Audio_Codec_DAC_NVIC_Configure()
{
	NVIC_InitTypeDef	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel 						= DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

/*
void DMA1_Stream5_IRQHandler()
{

    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_HTIF5))
    {
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_HTIF5);

        for(i=l;i<(32000+l);i++)
          Audio_Wav_Buffer_0[i] = Audio_Sample[i+(l*k)];
        
        k++;
        
        if(k%2 != 0)
        	l=32000;

        else
        	l=0;
    }
}

*/

int main()
{
	SystemInit();
	/*
	uint32_t j;
	for(j=0;j<32000;j++)
	{
		Audio_Wav_Buffer_0[i] = Audio_Sample[i];
	}
	*/

	Audio_Codec_DAC_GPIO_Configure();
	Audio_Codec_DAC_Timer_Configure();
	Audio_Codec_DAC_DAC_Configure();
	Audio_Codec_DAC_DMA_Configure();
	//Audio_Codec_DAC_NVIC_Configure();

	while(1);
}
