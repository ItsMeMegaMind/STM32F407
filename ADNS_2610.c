#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_GPIO.H>

#define Configuration	0x00
#define	Status			0x01
#define	Delta_Y			0x02
#define	Delta_X			0x03
#define	SQUAL			0x04
#define	Maximum_Pixel	0x05
#define	Minimum_Pixel	0x06
#define	Pixel_Sum		0x07
#define Pixel_Data		0x08

#define Always_Awake 	0x01

__IO uint32_t TimmingDelay;
uint8_t Buffer = 0x00;

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

void ADNS2610_GPIO_Configure_Out()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADNS2610_GPIO_Configure_In()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADNS2610_Write_Register(uint8_t Address, uint8_t Data)
{
	ADNS2610_GPIO_Configure_Out();

	uint8_t i;

	Address |= 0x80;

	for(i=8;i!=0;i--)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Address&(1<<(i-1)));
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}

	Delay(11);

	for(i=8;i!=0;i--)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Data&(1<<(i-1)));
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}
}

void ADNS2610_Read_Register(uint8_t Address)
{
	ADNS2610_GPIO_Configure_Out();

	uint8_t i;

	for(i=8;i!=0;i--)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Address&(1<<(i-1)));
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}

	ADNS2610_GPIO_Configure_In();
	Delay(11);

	for (i = 8; i != 0; i--)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	    Buffer |= (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) << (i-1)) ;
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}
}

void ADNS2610_Initialisation()
{
	ADNS2610_Write_Register(Configuration,Always_Awake);
}

int main()
{
	SysTick_Config(SystemCoreClock/1000000);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	Delay(1000);

	ADNS2610_Initialisation();
	while(1)
	{
		ADNS2610_Read_Register(Delta_Y);
	}
}
