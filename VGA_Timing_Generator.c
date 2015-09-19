// Incomplete VGA File
// Only to check required timing generated, VGA signal are unstable
// Incomplete VGA File

#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_TIM.H>
#include <STM32F4XX_DMA.H>
#include <STM32F4XX_DAC.H>
#include <STM32F4XX_GPIO.H>
#include <MISC.H>

#define	Pixel_Rate			25175000
#define V_Refresh_Rate 		31468
#define Timer_1_Period 		((SystemCoreClock/V_Refresh_Rate) - 1)
#define Timer_2_Period		525
#define	Timer_8_Period		((SystemCoreClock/Pixel_Rate) - 1)
#define H_Sync				640
#define V_Sync				2

#define Resolution			160

volatile uint16_t Display_Data_Buffer[Resolution] = {0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970,0,970};
volatile uint16_t Display_Blank_Buffer[Resolution] = {0};

uint16_t	Flag_HSync = 0, Flag_VSync = 0;

DMA_InitTypeDef DMA_InitSturture;

void VGA_GPIO_Configure()
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void VGA_DAC_Configure()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = Timer_8_Period;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);

	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
	TIM_Cmd(TIM8, ENABLE);

	DAC_InitTypeDef DAC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T8_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void VGA_Timer_Configuration()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef		TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_Period = Timer_1_Period;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_Pulse = H_Sync;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	TIM_Cmd(TIM1, ENABLE);

	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Gated);
	TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_Period = Timer_2_Period;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = V_Sync;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM2, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
}

void VGA_DMA_Configure()
{
	DMA_InitTypeDef DMA_InitSturture;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Stream5);
	DMA_InitSturture.DMA_Channel            = DMA_Channel_7;
	DMA_InitSturture.DMA_PeripheralBaseAddr = 0x40007408;
	DMA_InitSturture.DMA_Memory0BaseAddr    = (uint32_t)&Display_Data_Buffer;
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

	DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t)&Display_Data_Buffer, DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);

	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);

}

void VGA_NVIC_Configure()
{
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void DMA1_Stream5_IRQHandler()
{

    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5))
    {
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
        GPIO_ToggleBits(GPIOA, GPIO_Pin_3);

        Flag_HSync++;

        if(Flag_VSync == 479)
        {
        	DMA1_Stream5->M0AR = (uint32_t)&Display_Blank_Buffer;
        }

        else if(Flag_VSync == 4800)
        {
        	DMA1_Stream5->M1AR = (uint32_t)&Display_Blank_Buffer;
        }

        else if(Flag_VSync == 5249)
        {
        	DMA1_Stream5->M0AR = (uint32_t)&Display_Data_Buffer;
        }

        else if(Flag_VSync == 5250)
        {
        	DMA1_Stream5->M1AR = (uint32_t)&Display_Data_Buffer;
        	Flag_VSync = 0;
        	Flag_HSync = 0;
        }

        else if(Flag_HSync == 7)
        {
        	 DMA1_Stream5->M0AR = (uint32_t)&Display_Blank_Buffer;
        }

        else if(Flag_HSync == 8)
        {
        	DMA1_Stream5->M1AR = (uint32_t)&Display_Blank_Buffer;
        }

        else if(Flag_HSync == 9)
        {
        	DMA1_Stream5->M0AR = (uint32_t)&Display_Data_Buffer;
        }

        else if(Flag_HSync == 10)
        {
        	DMA1_Stream5->M1AR = (uint32_t)&Display_Data_Buffer;
        	Flag_HSync=0;
        	Flag_VSync++;
        }



    }
}

int main()
{
	SystemInit();

	VGA_GPIO_Configure();
	VGA_Timer_Configuration();
	VGA_DAC_Configure();
	VGA_DMA_Configure();
	VGA_NVIC_Configure();

	while(1);
}
