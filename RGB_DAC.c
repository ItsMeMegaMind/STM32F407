#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_DAC.H>
#include <STM32F4XX_GPIO.H>

__IO uint32_t TimingDelay;

void DAC_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC_Configure()
{
	DAC_InitTypeDef DAC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_2, ENABLE);
}

void DAC_Set_Output(int Value_1, int Value_2)
{
	DAC_SetChannel1Data(DAC_Align_12b_R, Value_1);
	DAC_SetChannel2Data(DAC_Align_12b_R, Value_2);
}

void Delay(__IO uint32_t Time)
{
  TimingDelay = Time;
  while(TimingDelay !=0);
}

void SysTick_Handler(void)
{
  if(TimingDelay !=0)
  {
    TimingDelay --;
   }
}

int main()
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000);

	DAC_GPIO_Configure();
	DAC_Configure();

	int i = 0;

	while(1)
	{
		for(i=2500;i<4050;i++)
		{
			DAC_Set_Output(i,i);
			Delay(5);
		}
	}

}
