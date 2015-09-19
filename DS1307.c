#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_I2C.H>
#include <STM32F4XX_GPIO.H>

#define DS1307_Address			0xD0
#define STM32F4XX_Address  		0x00

typedef struct Time
{
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hours;

}Time;

typedef struct Calendar
{
	uint8_t Day_Name;
	uint8_t Day_Number;
	uint8_t Month;
	uint16_t Year;

}Calendar;

Time Time_1;
Calendar Calendar_1;
char Buffer_LCD[14];

void DS1307_GPIO_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void DS1307_I2C_Configure()
{
	I2C_InitTypeDef I2C_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = STM32F4XX_Address;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitStructure);
    I2C_Cmd(I2C2, ENABLE);
}

void DS1307_Set_Time()
{
	uint8_t i,Buffer[8] = { 0x00, 0x35, 0x19, 0x04, 0x0E, 0x01, 0x15, 0x00 };

	I2C_AcknowledgeConfig(I2C2, ENABLE);

	while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

	I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, DS1307_Address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, 0x00);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    for (i=0;i<8;i++)
    {
    	I2C_SendData(I2C2, Buffer[i]);
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    I2C_GenerateSTOP(I2C2, ENABLE);
    Delay(0xFF);
}


void DS1307_Read_Time()
{
	uint8_t i;
	uint8_t Buffer[8];

    I2C_AcknowledgeConfig(I2C2, ENABLE);

    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, DS1307_Address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, 0x00);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, DS1307_Address, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    for (i=0;i<8;i++)
    {
    	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
        Buffer[i] = I2C_ReceiveData(I2C2);
        if (i == 6)
        {
        	I2C_NACKPositionConfig(I2C2, I2C_NACKPosition_Current);
            I2C_AcknowledgeConfig(I2C2, DISABLE);
        }
    }

    I2C_GenerateSTOP(I2C2, ENABLE);
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF));

    Time_1.Seconds = ((((Buffer[0]&0xF0)>>4)*10) + (Buffer[0]&0x0F));
    Time_1.Minutes = ((((Buffer[1]&0xF0)>>4)*10) + (Buffer[1]&0x0F));
    Time_1.Hours   = ((((Buffer[2]&0x30)>>4)*10) + (Buffer[2]&0x0F));

    Calendar_1.Day_Name   = (Buffer[3]&0x0F);
    Calendar_1.Day_Number = ((((Buffer[4]&0x30)>>4)*10) + (Buffer[4]&0x0F));
    Calendar_1.Month 	  = ((((Buffer[5]&0x10)>>4)*10) + (Buffer[5]&0x0F));
    Calendar_1.Year		  =	((((Buffer[6]&0xF0)>>4)*10) + (Buffer[6]&0x0F));
}

void DS1307_Display_Date_Calendar()
{
	LCD_Clear();

	LCD_Set_XY(0,0);
	sprintf(Buffer_LCD, "%d", Time_1.Hours);
	LCD_Write_String(Buffer_LCD);
	LCD_Write_Character(':');
	sprintf(Buffer_LCD, "%d", Time_1.Minutes);
	LCD_Write_String(Buffer_LCD);
	LCD_Write_Character(':');
	sprintf(Buffer_LCD, "%d", Time_1.Seconds);
	LCD_Write_String(Buffer_LCD);

	LCD_Set_XY(0,2);
	sprintf(Buffer_LCD, "%d", Calendar_1.Day_Number);
	LCD_Write_String(Buffer_LCD);
	LCD_Write_Character('/');
	sprintf(Buffer_LCD, "%d", Calendar_1.Month);
	LCD_Write_String(Buffer_LCD);
	LCD_Write_String("/20");
	sprintf(Buffer_LCD, "%d", Calendar_1.Year);
	LCD_Write_String(Buffer_LCD);

	LCD_Set_XY(0,4);
	switch(Calendar_1.Day_Name)
	{
		case 1 : LCD_Write_String("Sunday");
					break;
		case 2 : LCD_Write_String("Monday");
					break;
		case 3 : LCD_Write_String("Tuesday");
					break;
		case 4 : LCD_Write_String("Wednesday");
					break;
		case 5 : LCD_Write_String("Thursday");
					break;
		case 6 : LCD_Write_String("Friday");
					break;
		case 7 : LCD_Write_String("Saturday");
					break;
	}

}

int main(void)
{
	LCD_GPIO_Configure();
	LCD_Initialise();
	DS1307_GPIO_Configure();
	DS1307_I2C_Configure();
	DS1307_Set_Time();
	while(1)
	{
		DS1307_Read_Time();
		DS1307_Display_Date_Calendar();
	}
}
