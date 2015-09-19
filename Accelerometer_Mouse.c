#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_SPI.H>
#include <STM32F4XX_GPIO.H>
#include <STM32F4XX_USART.H>
#include <ARM_MATH.H>

#define Sample_Count 100

int8_t 	MSB_X, LSB_X, MSB_Y, LSB_Y, MSB_Z, LSB_Z;
int8_t	X_Dir = 0, Y_Dir = 0;
int16_t	X_C = 0, Y_C = 0, Z_C = 0;



float32_t X_Average = 0, Y_Average = 0, Z_Average = 0, X_AB = 0, Y_AB = 0, Z_AB = 0;
float32_t X_Array[Sample_Count],Y_Array[Sample_Count],Z_Array[Sample_Count];

__IO uint32_t TimingDelay;

void LIS3DSH_GPIO_Configure()
{
	 GPIO_InitTypeDef GPIO_InitStructure;

	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE);

	 GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	 GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);

	 GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	 GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_Init(GPIOE, &GPIO_InitStructure);

	 GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	 GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	 GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	 GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

void LIS3DSH_SPI_Configure()
{
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);

	LIS3DSH_SPI_Send(0x20, 0x67);
	LIS3DSH_SPI_Send(0x24, 0x20);
}

void LIS3DSH_SPI_Send(uint8_t Address, uint8_t Data)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_3);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, Address);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(SPI1);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, Data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(SPI1);

	GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

uint8_t LIS3DSH_SPI_Read(uint8_t Address)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_3);
	Address = 0x80|Address;

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, Address);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(SPI1);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0x00);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	GPIO_SetBits(GPIOE, GPIO_Pin_3);

	return SPI_I2S_ReceiveData(SPI1);
}

void UART_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void UART_Configure()
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);
}

void UART_Send_Character(char Data)
{
	while (!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
	USART3->DR = Data;
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

int main(void)
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000000);

	LIS3DSH_GPIO_Configure();
	LIS3DSH_SPI_Configure();

	UART_GPIO_Configure();
	UART_Configure();

	uint16_t i = 0;

	while(1)
	{
		for(i=0;i<Sample_Count;i++)
	    {
			MSB_X = LIS3DSH_SPI_Read(0x29);
			LSB_X = LIS3DSH_SPI_Read(0x28);
			MSB_Y = LIS3DSH_SPI_Read(0x2B);
			LSB_Y = LIS3DSH_SPI_Read(0x2A);
			MSB_Z = LIS3DSH_SPI_Read(0x2D);
			LSB_Z = LIS3DSH_SPI_Read(0x2C);

			X_Array[i] = (float32_t)((MSB_X << 8) | (LSB_X));
			Y_Array[i] = (float32_t)((MSB_Y << 8) | (LSB_Y));
			Z_Array[i] = (float32_t)((MSB_Z << 8) | (LSB_Z));

			Delay(50);
	    }

		arm_mean_f32(X_Array, Sample_Count, &X_Average);
		arm_mean_f32(Y_Array, Sample_Count, &Y_Average);
		arm_mean_f32(Z_Array, Sample_Count, &Z_Average);

		if(X_Average > 0)
		X_Dir = 0;

		if(X_Average < 0)
		X_Dir = 1;

		if(Y_Average > 0)
		Y_Dir = 0;

		if(Y_Average < 0)
		Y_Dir = 1;

		arm_abs_f32(&X_Average,&X_AB,1);
		arm_abs_f32(&Y_Average,&Y_AB,1);

		X_C = (int16_t)X_AB;
		Y_C = (int16_t)Y_AB;

		X_C = X_C/100;
		Y_C = Y_C/100;

		if(X_C >= 12)
		{
			X_C = 0;
		}

		if(Y_C >= 12)
		{
			Y_C = 0;
		}

		UART_Send_Character(X_C);
		UART_Send_Character(Y_C);
		UART_Send_Character(X_Dir);
		UART_Send_Character(Y_Dir);

	}
}
