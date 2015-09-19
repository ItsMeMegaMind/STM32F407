// Incomplete Project File
// Timing Error
// Incomplete Project File
#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_GPIO.H>
#include <STM32F4XX_EXTI.H>
#include <STM32F4XX_SYSCFG.H>
#include <MISC.H>
#include <MATH.H>

#define Reset				0xFF
#define Set_Defaults 		0xF6
#define Enable_Reporting 	0xF4
#define Disable_Reporting	0xF5
#define Set_Sample_Rate		0xF3
#define Set_Resolution 		0xE8
#define Set_Scaling_2		0xE7
#define Set_Scaling_1		0xE6
#define Status_Request 		0xE9

uint8_t Flag_Send = 0, Flag_Recieve = 0;
uint8_t Index = 1;
char Buffer[44];
char Address = Enable_Reporting;

GPIO_InitTypeDef GPIO_InitStruct;

void PS2_GPIO_Configure_1()
{
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_SetBits(GPIOA, GPIO_Pin_0);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void PS2_GPIO_Configure_2()
{
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void PS2_Interrupt_Configure()
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

char PS2_Generate_Parity(char Data)
{

}

void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		if(Flag_Send == 1)
		{
			if(Index>=1 && Index<=8)
			{
				GPIO_WriteBit(GPIOA, GPIO_Pin_1, Address & 0x01);
				Address = Address >> 1;
				Index++;
			}
			else if(Index == 9)
			{
				GPIO_WriteBit(GPIOA, GPIO_Pin_1, 0);
				Index++;
			}
			else if(Index == 10)
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_1);
				Index++;
			}
			else if(Index == 11)
			{
				Index++;
			}
			else if(Index == 12)
			{
				Index = 0;
				Flag_Send = 0;
				Flag_Recieve = 1;
			}
		}

		else if(Flag_Recieve == 1)
		{
			Buffer[43-Index] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
			Index++;
			if(Index == 43)
			{
				Index = 0;
			}
		}

	}

	EXTI_ClearITPendingBit(EXTI_Line0);
}

void PS2_Send_Byte()
{
	Flag_Send = 1;
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	Delay(0xFF);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
	PS2_GPIO_Configure_2();
}

int main()
{
	Delay(0xFFFF);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	PS2_Interrupt_Configure();
	PS2_GPIO_Configure_1();
	PS2_Send_Byte();
	while(1)
	{

	}
}
