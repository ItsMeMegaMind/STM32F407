#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_GPIO.H>
#include <STM32F4XX_TIM.H>
#include <MISC.H>

__IO uint32_t TimingDelay;

long int Time = 0;
long int C1 = 0;
long int C2 = 0;
int Flag = 0,Data = 0;
char Buffer[5];

void Ultrasonic_GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	GPIO_Init(GPIOE, &GPIO_InitStructure) ;

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1) ;
}


void Ultrasonic_Timer_Configure(void)
{
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef  			TIM_ICInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 84-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1,&TIM_ICInitStructure);

	TIM_Cmd(TIM1,ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);
}

void Ultrasonic_NVIC_Configure(void)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
void Ultrasonic_Trigger_Toggle(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_10);
	Delay(12);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
}

void Delay(__IO uint32_t Time)
{
	TimingDelay = Time;
	while(TimingDelay !=0);
}

// Interrupt Service Routine

void TIM1_CC_IRQHandler()
{
	if(TIM_GetITStatus(TIM1,TIM_IT_CC2) == SET)
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_CC2);
		if(Flag == 0)
		{
			C1 = TIM_GetCapture2(TIM1);
			Flag = 1;
			Data = 0;
		}

		else if(Flag == 1)
		{
			C2 = TIM_GetCapture2(TIM1);
			Flag = 0;
			Data = 1;
		}
	}
}

void SysTick_Handler(void)
{
	if(TimingDelay !=0)
	{
		TimingDelay --;
	}
}

int main(void)
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000);

	PCD8544_GPIO_Configure();
	PCD8544_Initialise();

	Ultrasonic_GPIO_Configure();
	Ultrasonic_NVIC_Configure();
	Ultrasonic_Timer_Configure();

	while(1)
	{
		if (Data == 1)
		{
			if (C2 > C1)
		    {
		        Time = C2 - C1;
		    }
		    else
		    {
		    	Time = ((0xFFFF - C1) + C2);
		    }

			Time = (Time*3)/325;
			Data = 0;
		    sprintf(Buffer, "%d ", Time);

		    PCD8544_Write_String(Buffer,0);
		    PCD8544_Set_XY(0,0);

		    Delay(1000);
		    PCD8544_Clear();
		    Ultrasonic_Trigger_Toggle();
		}
	}
}
