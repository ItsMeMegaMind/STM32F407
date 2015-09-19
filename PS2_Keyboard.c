#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_GPIO.H>
#include <STM32F4XX_EXTI.H>
#include <STM32F4XX_SYSCFG.H>
#include <MISC.H>
#include <MATH.H>

__IO uint32_t TimingDelay;

uint8_t Flag_1 = 0, Index = 0, Key_Status = 0, Shift_Status = 0;
uint8_t X = 0,Y = 0;
uint8_t Buffer[8],Data=0;

char Scan_Codes[][2] =
{
		{22,49},
		{30,50},
		{38,51},
		{37,52},
		{46,53},
		{54,54},
		{61,55},
		{62,56},
		{70,57},
		{69,48},
		{78,45},
		{85,61},
		{21,113},
		{29,119},
		{36,101},
		{45,114},
		{44,116},
		{53,121},
		{60,117},
		{67,105},
		{68,111},
		{77,112},
		{84,91},
		{91,93},
		{28,97},
		{27,115},
		{35,100},
		{43,102},
		{52,103},
		{51,104},
		{59,106},
		{66,107},
		{75,108},
		{76,59},
		{82,39},
		{26,122},
		{34,120},
		{33,99},
		{42,118},
		{50,98},
		{49,110},
		{58,109},
		{65,44},
		{73,46},
		{74,47},
		{41,32}
};

void PS2_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void PS2_Interrupt_Configure()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Delay(__IO uint32_t Time)
{
  TimingDelay = Time;
  while(TimingDelay !=0);
}

void PS2_Data_Display()
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		Data = Data + (pow(2,i)*Buffer[i]);
	}

	Key_Status = 1;

	switch(Data)
	{
		case 112:	Key_Status = 0;
					break;

		case 18:	Shift_Status = 1;
					break;

		case 89:	Shift_Status = 1;
					break;

		case 90:	X = 0;
					Y++;
					if(Y == 6)
					{
						Y = 0;
					}

					PCD8544_Set_XY(X,Y);
					break;

		case 102:	if(X == 0)
					{
						if(Y == 0)
						{
							Y = 0;
							X = 0;
						}

						else
						{
							X = 13;
							Y--;
						}
					}

					else
					{
						X--;
					}

					PCD8544_Set_XY(X,Y);
					PCD8544_Write_Character(' ');
					PCD8544_Set_XY(X,Y);
					break;

		default:	for(i=0;i<46;i++)
					{
						if(Scan_Codes[i][0] == Data)
						{
							PCD8544_Write_Character(Scan_Codes[i][1]);
						}
					}

					if(X == 13)
					{
						X = 0;
						Y++;
						if(Y == 6)
						{
							Y = 0;
						}
					}

					else
					{
						X++;
					}

					break;
	}

	Data = 0;
}

void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{

		if(Index == 0)
		{
			Index++;
		}

		else if(Index >= 1 && Index <= 8)
		{
			Buffer[Index-1] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
			Index++;
		}

		else if(Index == 9)
		{
			Index++;
		}

		else if(Index == 10)
		{
			Index = 0;
			Flag_1 = 1;
		}

		EXTI_ClearITPendingBit(EXTI_Line0);
	}

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

	PCD8544_GPIO_Configure();
	PCD8544_Initialise();

	PS2_GPIO_Configure();
	PS2_Interrupt_Configure();

	while(1)
	{
		if(Flag_1 == 1)
		{
			PS2_Data_Display();
			Flag_1 = 0;
		}
	}
}
