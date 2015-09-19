#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_TIM.H>
#include <STM32F4XX_GPIO.H>

#define Angle_Max	180
#define Angle_Min	0
#define Pulse_Min	499
#define Pulse_Max	2499

#define Resolution	(Pulse_Max - Pulse_Min)/(Angle_Max - Angle_Min)
#define Offset		499

#define Pi 			3.14159265358979

__IO uint32_t TimmingDelay;

void Servo_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9,  GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void Servo_Timer_Configure()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitStructure.TIM_Prescaler = 167;
	TIM_TimeBaseInitStructure.TIM_Period = 19999;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
    TIM_Cmd(TIM1, ENABLE);
}

void Servo_PWM_Configure()
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OCInitStructure.TIM_Pulse = 499;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse = 499;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Servo_Go_Degree(float Angle, uint8_t Servo)
{
	uint16_t Pulse;

	if(Servo == 0)
	{
		Pulse = (uint16_t)((Angle*Resolution) + Offset);
		TIM_SetCompare1(TIM1, Pulse);
	}

	else if(Servo == 1)
	{
		Pulse = (uint16_t)((Angle*Resolution) + Offset);
		TIM_SetCompare2(TIM1, Pulse);
	}
}

void Delay(__IO uint32_t time)
{
  TimmingDelay = time;
  while(TimmingDelay !=0);
}

void SysTick_Handler(void)
{
  if(TimmingDelay !=0)
  {
    TimmingDelay --;
   }
}

int main(void)
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000);

	Servo_GPIO_Configure();
	Servo_Timer_Configure();
	Servo_PWM_Configure();

	uint8_t i = 0;
	
	while(1)
	{
		
		for(i=0;i<=180;i=i+15)
		{
			Servo_Go_Degree(i,0);
			Servo_Go_Degree(i,1);
			Delay(1500);
		}
	}
}	
