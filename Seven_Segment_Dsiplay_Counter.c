#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_SPI.H>
#include <STM32F4XX_GPIO.H>

__IO uint32_t TimingDelay;

uint16_t Numbers[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

void HC595_GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void HC595_SPI_Configure(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}

void HC595_Send(uint8_t Data)
{
	SPI_I2S_SendData(SPI2, Data);
	while (!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void Display_Number(uint32_t Data)
{
	uint8_t i=0;
	uint8_t Select=0;
	uint8_t Buffer[8];

	while(Data>=0)
	{
		if(i==8)
		{
			break;
		}
		Buffer[7-i] = Data%10;
		Data /= 10;
		i++;
	}

	for(i=0;i<8;i++)
	{
		Select=pow(2,7-i);
		HC595_Send(Numbers[Buffer[i]]);
		HC595_Send(Select);
		Delay(1000);
	}
}

void Delay(__IO uint32_t Time)
{
  TimingDelay = Time;
  while(TimingDelay !=0);
}

// Interrupt Service Routine(s)

void SysTick_Handler(void)
{
  if(TimingDelay !=0)
  {
    TimingDelay --;
   }
}


int main(void)
{
	SystemInit();	// Setting System Clocks
	SysTick_Config(SystemCoreClock/1000000); // Setting Systick to Micro Seconds

	HC595_GPIO_Configure();
	HC595_SPI_Configure();

	uint32_t i;

	for(i=0;i<99999999;i++)
	{
		Display_Number(i);
		Delay(10000);
	}
}
