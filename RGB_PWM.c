#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_TIM.H>
#include <STM32F4XX_GPIO.H>
#include <MATH.H>

void RGB_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void RGB_Timer_Configure()
{
	TIM_TimeBaseInitTypeDef TIM_BaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_BaseStructure.TIM_Prescaler = 0;
    TIM_BaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStructure.TIM_Period = 8399;
    TIM_BaseStructure.TIM_ClockDivision = 0;
    TIM_BaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_BaseStructure);
    TIM_Cmd(TIM3, ENABLE);
}

void RGB_PWM_Configure(uint16_t R, uint16_t G, uint16_t B)
{
	TIM_OCInitTypeDef TIM_OCStructure;
	TIM_OCStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OCStructure.TIM_Pulse = R;
	TIM_OC1Init(TIM3, &TIM_OCStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OCStructure.TIM_Pulse = G;
	TIM_OC2Init(TIM3, &TIM_OCStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OCStructure.TIM_Pulse = B;
	TIM_OC3Init(TIM3, &TIM_OCStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

void Delay(__IO uint32_t nCount)
{
	while(nCount--)
	{
	}
}

int main()
{
	RGB_GPIO_Configure();
	RGB_Timer_Configure();
	int i,j,k;
	while(1)
	{
		for(i=0;i<8399;i++)
		{
			RGB_PWM_Configure(i,4199,8399-i);
			Delay(0xFFF);
		}

	}
}
