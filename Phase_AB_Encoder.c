#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_GPIO.H>
#include <STM32F4XX_EXTI.H>
#include <STM32F4XX_SYSCFG.H>
#include <MISC.H>

#define ILI9341_COLOR_WHITE			0xFFFF
#define ILI9341_COLOR_BLACK			0x0000
#define ILI9341_COLOR_RED			0xF800
#define ILI9341_COLOR_GREEN			0x07E0
#define ILI9341_COLOR_GREEN_2		0xB723
#define ILI9341_COLOR_BLUE			0x001F
#define ILI9341_COLOR_BLUE_2		0x051D
#define ILI9341_COLOR_YELLOW		0xFFE0
#define ILI9341_COLOR_ORANGE		0xFBE4
#define ILI9341_COLOR_CYAN			0x07FF
#define ILI9341_COLOR_MAGENTA		0xA254
#define ILI9341_COLOR_GRAY			0x7BEF
#define ILI9341_COLOR_BROWN			0xBBCA
#define ILI9341_TRANSPARENT			0x80000000

__IO uint32_t TimingDelay;

char Buffer[10];
uint8_t Flag_1 = 0, Flag_2 = 0;
uint16_t Counter = 0;

void Encoder_Configure_All(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);

	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
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

void EXTI3_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		Flag_1 = 1;

		if(Flag_2 == 1)
		{
			Flag_1 = 0;
			Flag_2 = 0;
			Counter++;
		}

		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

void EXTI4_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		Flag_2 = 1;

		if(Flag_1 == 1)
		{
			Flag_1 = 0;
			Flag_2 = 0;
			Counter--;
		}

		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

int main(void)
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000000);

	ILI9341_GPIO_Configure();
	ILI9341_SPI_Configure();
	ILI9341_Initialise();


	Encoder_Configure_All();

	ILI9341_Fill_Background(ILI9341_COLOR_BLUE);

	while (1)
	{
		sprintf(Buffer, "%d", Counter);
		ILI9341_Write_String(0,5,ILI9341_COLOR_MAGENTA,ILI9341_COLOR_BLUE,Buffer);
	}
}
