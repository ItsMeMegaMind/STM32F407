#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_GPIO.H>
#include <STM32F4XX_USART.H>
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

#define Baud_Rate 9600

__IO uint32_t TimingDelay;

char Data;
char Buffer[512];

uint8_t Flag = 0,Flag_Ignore = 0,Index = 0, i = 0, j = 0;
uint16_t X = 0, Y = 0;

void Delay(__IO uint32_t Time)
{
  TimingDelay = Time;
  while(TimingDelay !=0);
}

void ESP8266_GPIO_Configure()
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

void ESP8266_USART_Configure()
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_InitStructure.USART_BaudRate = Baud_Rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);
}

void ESP8266_NVIC_Configure()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART3, ENABLE);
}

void ESP8266_Send_Character(char Data_Tx)
{
	while (!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
	USART3->DR = Data_Tx;
}

void ESP8266_Send_String(char *Data_Tx)
{
	char Element;
	while (*Data_Tx != '\0')
	{
		Element = *Data_Tx;
		ESP8266_Send_Character(Element);
		Data_Tx++;
	}
}

void ESP8266_Check_USART()
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT\r\n");
}

void ESP8266_Reset_Module()
{
	Flag_Ignore = 1;
	ESP8266_Send_String("AT+RST\r\n");
}

void ESP8266_Get_Firmware()
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+GMR\r\n");
}

void ESP8266_Get_Mode()
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+CWMODE=?\r\n");
}

void ESP8266_Set_Mode(uint8_t Mode)
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+CWMODE=");
	ESP8266_Send_Character(Mode);
	ESP8266_Send_String("\r\n");
}

void ESP8266_Join_Access_Point(char *SSID, char *Password)
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+CWJAP=");
	ESP8266_Send_Character('"');
	ESP8266_Send_String(SSID);
	ESP8266_Send_Character('"');
	ESP8266_Send_Character(',');
	ESP8266_Send_Character('"');
	ESP8266_Send_String(Password);
	ESP8266_Send_Character('"');
	ESP8266_Send_String("\r\n");
}

void ESP8266_Quit_Access_Point()
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+CWQAP\r\n");
}

void ESP8266_Get_IP()
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+CIFSR\r\n");
}

void ESP8266_Configure_TCP_UDP(uint8_t Number)
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+CIPMUX=");
	ESP8266_Send_Character(Number);
	ESP8266_Send_String("/r/n");
}

void ESP8266_Setup_Server(uint8_t Mode,uint16_t Port)
{
	char Buffer_Port[6];
	sprintf(Buffer_Port,"%d", Port);

	Flag_Ignore = 0;

	ESP8266_Send_String("AT+CIPSERVER=");
	ESP8266_Send_Character(Mode);
	ESP8266_Send_Character(',');
	ESP8266_Send_String(Buffer_Port);
	ESP8266_Send_String("\r\n");
}

void ESP8266_Get_Baudrate()
{
	Flag_Ignore = 0;
	ESP8266_Send_String("AT+CIOBAUD?\r\n");
}

void ESP8266_Update_Baudrate(uint32_t Baudrate)
{
	char Buffer_Baudrate[10];
	sprintf(Buffer_Baudrate, "%d", Baudrate);

	Flag_Ignore = 0;

	ESP8266_Send_String("AT+CIOBAUD=");
	ESP8266_Send_String(Buffer_Baudrate);
	ESP8266_Send_String("\r\n");
}

void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Buffer[i] = USART3->DR;

		if(Buffer[i] == '\n')
		{
			Index++;

			if(Index == 4)
			{
				Index = 0;
				i = 0;
				ILI9341_Fill_Background(ILI9341_COLOR_BLACK);
				ILI9341_Write_String(0, 5, ILI9341_COLOR_RED, ILI9341_COLOR_BLACK, Buffer);

			}
		}

		else
		{
			i++;
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

int main()
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000000);

	ILI9341_GPIO_Configure();
	ILI9341_SPI_Configure();
	ILI9341_Initialise();

	ESP8266_GPIO_Configure();
	ESP8266_USART_Configure();
	ESP8266_NVIC_Configure();

	ILI9341_Fill_Background(ILI9341_COLOR_BLACK);

	while(1);

}
