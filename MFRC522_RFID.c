// Ported using official Arduino MFRC522 library
// http://playground.arduino.cc/Learning/MFRC522

#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_SPI.H>
#include <STM32F4XX_GPIO.H>

#define PCD_IDLE						0x00   //NO action; Cancel the current command
#define PCD_AUTHENT						0x0E   //Authentication Key
#define PCD_RECEIVE						0x08   //Receive Data
#define PCD_TRANSMIT					0x04   //Transmit data
#define PCD_TRANSCEIVE					0x0C   //Transmit and receive data,
#define PCD_RESETPHASE					0x0F   //Reset
#define PCD_CALCCRC						0x03   //CRC Calculate

#define PICC_REQIDL						0x26   // find the antenna area does not enter hibernation
#define PICC_REQALL						0x52   // find all the cards antenna area
#define PICC_ANTICOLL					0x93   // anti-collision
#define PICC_SElECTTAG					0x93   // election card
#define PICC_AUTHENT1A					0x60   // authentication key A
#define PICC_AUTHENT1B					0x61   // authentication key B
#define PICC_READ						0x30   // Read Block
#define PICC_WRITE						0xA0   // write block
#define PICC_DECREMENT					0xC0   // debit
#define PICC_INCREMENT					0xC1   // recharge
#define PICC_RESTORE					0xC2   // transfer block data to the buffer
#define PICC_TRANSFER					0xB0   // save the data in the buffer
#define PICC_HALT						0x50   // Sleep

#define MFRC522_REG_RESERVED00			0x00
#define MFRC522_REG_COMMAND				0x01
#define MFRC522_REG_COMM_IE_N			0x02
#define MFRC522_REG_DIV1_EN				0x03
#define MFRC522_REG_COMM_IRQ			0x04
#define MFRC522_REG_DIV_IRQ				0x05
#define MFRC522_REG_ERROR				0x06
#define MFRC522_REG_STATUS1				0x07
#define MFRC522_REG_STATUS2				0x08
#define MFRC522_REG_FIFO_DATA			0x09
#define MFRC522_REG_FIFO_LEVEL			0x0A
#define MFRC522_REG_WATER_LEVEL			0x0B
#define MFRC522_REG_CONTROL				0x0C
#define MFRC522_REG_BIT_FRAMING			0x0D
#define MFRC522_REG_COLL				0x0E
#define MFRC522_REG_RESERVED01			0x0F
#define MFRC522_REG_RESERVED10			0x10
#define MFRC522_REG_MODE				0x11
#define MFRC522_REG_TX_MODE				0x12
#define MFRC522_REG_RX_MODE				0x13
#define MFRC522_REG_TX_CONTROL			0x14
#define MFRC522_REG_TX_AUTO				0x15
#define MFRC522_REG_TX_SELL				0x16
#define MFRC522_REG_RX_SELL				0x17
#define MFRC522_REG_RX_THRESHOLD		0x18
#define MFRC522_REG_DEMOD				0x19
#define MFRC522_REG_RESERVED11			0x1A
#define MFRC522_REG_RESERVED12			0x1B
#define MFRC522_REG_MIFARE				0x1C
#define MFRC522_REG_RESERVED13			0x1D
#define MFRC522_REG_RESERVED14			0x1E
#define MFRC522_REG_SERIALSPEED			0x1F
#define MFRC522_REG_RESERVED20			0x20
#define MFRC522_REG_CRC_RESULT_M		0x21
#define MFRC522_REG_CRC_RESULT_L		0x22
#define MFRC522_REG_RESERVED21			0x23
#define MFRC522_REG_MOD_WIDTH			0x24
#define MFRC522_REG_RESERVED22			0x25
#define MFRC522_REG_RF_CFG				0x26
#define MFRC522_REG_GS_N				0x27
#define MFRC522_REG_CWGS_PREG			0x28
#define MFRC522_REG__MODGS_PREG			0x29
#define MFRC522_REG_T_MODE				0x2A
#define MFRC522_REG_T_PRESCALER			0x2B
#define MFRC522_REG_T_RELOAD_H			0x2C
#define MFRC522_REG_T_RELOAD_L			0x2D
#define MFRC522_REG_T_COUNTER_VALUE_H	0x2E
#define MFRC522_REG_T_COUNTER_VALUE_L	0x2F
#define MFRC522_REG_RESERVED30			0x30
#define MFRC522_REG_TEST_SEL1			0x31
#define MFRC522_REG_TEST_SEL2			0x32
#define MFRC522_REG_TEST_PIN_EN			0x33
#define MFRC522_REG_TEST_PIN_VALUE		0x34
#define MFRC522_REG_TEST_BUS			0x35
#define MFRC522_REG_AUTO_TEST			0x36
#define MFRC522_REG_VERSION				0x37
#define MFRC522_REG_ANALOG_TEST			0x38
#define MFRC522_REG_TEST_ADC1			0x39
#define MFRC522_REG_TEST_ADC2			0x3A
#define MFRC522_REG_TEST_ADC0			0x3B
#define MFRC522_REG_RESERVED31			0x3C
#define MFRC522_REG_RESERVED32			0x3D
#define MFRC522_REG_RESERVED33			0x3E
#define MFRC522_REG_RESERVED34			0x3F

#define MFRC522_DUMMY					0x00

#define MFRC522_MAX_LEN					16

typedef enum
{
	MI_OK = 0,
	MI_NOTAGERR,
	MI_ERR
} MFRC522_Status_t;

__IO uint32_t TimingDelay;

uint8_t i = 0;

void MFRC522_GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_2);
}

void MFRC522_SPI_Configure()
{
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_Cmd(SPI3, ENABLE);
}

uint8_t MFRC522_SPI_Send(uint8_t Data)
{
	SPI3->DR = Data;
	while (!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE));
	while (!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE));
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
	return SPI3->DR;
}

void MFRC522_WriteRegister(uint8_t addr, uint8_t val)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);

	MFRC522_SPI_Send((addr << 1) & 0x7E);
	MFRC522_SPI_Send(val);

	GPIO_SetBits(GPIOB, GPIO_Pin_2);
}

uint8_t MFRC522_ReadRegister(uint8_t addr)
{
	uint8_t val;

	GPIO_ResetBits(GPIOB, GPIO_Pin_2);

	MFRC522_SPI_Send(((addr << 1) & 0x7E) | 0x80);
	val =  MFRC522_SPI_Send(MFRC522_DUMMY);

	GPIO_SetBits(GPIOB, GPIO_Pin_2);

	return val;
}

void Delay(__IO uint32_t Time)
{
  TimingDelay = Time;
  while(TimingDelay !=0);
}

MFRC522_Status_t MFRC522_Check(uint8_t* id);
MFRC522_Status_t MFRC522_Compare(uint8_t* CardID, uint8_t* CompareID);
void MFRC522_SetBitMask(uint8_t reg, uint8_t mask);
void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask);
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);
void MFRC522_Reset(void);
MFRC522_Status_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType);
MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen);
MFRC522_Status_t MFRC522_Anticoll(uint8_t* serNum);
void MFRC522_CalculateCRC(uint8_t* pIndata, uint8_t len, uint8_t* pOutData);
uint8_t MFRC522_SelectTag(uint8_t* serNum);
MFRC522_Status_t MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum);
MFRC522_Status_t MFRC522_Read(uint8_t blockAddr, uint8_t* recvData);
MFRC522_Status_t MFRC522_Write(uint8_t blockAddr, uint8_t* writeData);
void MFRC522_Halt(void);
void MFRC522_Init(void);

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

    Delay(100);

    MFRC522_GPIO_Configure();
    MFRC522_SPI_Configure();

    Delay(100);

    MFRC522_Init();

    GPIO_SetBits(GPIOD, GPIO_Pin_15);
    Delay(1000);
    GPIO_ResetBits(GPIOD, GPIO_Pin_15);

    uint8_t CardID[5];

    while (1)
    {
        if (MFRC522_Check(CardID) == MI_OK)
        {
            GPIO_SetBits(GPIOD, GPIO_Pin_15);
            Delay(1000);
            i = 1;
        }

        else
        {
            GPIO_ResetBits(GPIOD, GPIO_Pin_15);
            i = 0;
        }
    }
}

MFRC522_Status_t MFRC522_Check(uint8_t* id)
{
	MFRC522_Status_t status;
	status = MFRC522_Request(PICC_REQIDL, id);

	if (status == MI_OK)
	{
		status = MFRC522_Anticoll(id);
	}
	 MFRC522_Halt();

	return status;
}

MFRC522_Status_t MFRC522_Compare(uint8_t* CardID, uint8_t* CompareID)
{
	uint8_t i;

	for (i = 0; i < 5; i++)
	{
		if (CardID[i] != CompareID[i])
		{
			return MI_ERR;
		}
	}

	return MI_OK;
}


void MFRC522_SetBitMask(uint8_t reg, uint8_t mask)
{
	MFRC522_WriteRegister(reg,  MFRC522_ReadRegister(reg) | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
	MFRC522_WriteRegister(reg,  MFRC522_ReadRegister(reg) & (~mask));
}

void MFRC522_AntennaOn(void)
{
	uint8_t temp;

	temp = MFRC522_ReadRegister(MFRC522_REG_TX_CONTROL);
	if (!(temp & 0x03))
	{
		MFRC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
	}
}

void MFRC522_AntennaOff(void)
{
	MFRC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_Reset(void)
{
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

MFRC522_Status_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType)
{
	MFRC522_Status_t status;
	uint16_t backBits;

	MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);

	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10))
	{
		status = MI_ERR;
	}

	return status;
}

MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen)
{
	MFRC522_Status_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint16_t i;

	switch (command)
	{
		case PCD_AUTHENT:
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}

		case PCD_TRANSCEIVE:
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}

		default:
		break;
	}

	MFRC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	MFRC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);

	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

	for (i = 0; i < sendLen; i++)
	{
		MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);
	}

	MFRC522_WriteRegister(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE)
	{
		MFRC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);
	}

	i = 2250;	//i = 2000;

	do
	{
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = MFRC522_ReadRegister(MFRC522_REG_COMM_IRQ);
		i--;
	}

	while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	MFRC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);

	if (i != 0)
	{
		if (!(MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B))
		{
			status = MI_OK;
			if (n & irqEn & 0x01)
			{
				status = MI_NOTAGERR;
			}

			if (command == PCD_TRANSCEIVE)
			{
				n = MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
				lastBits = MFRC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;

				if (lastBits)
				{
					*backLen = (n - 1) * 8 + lastBits;
				}

				else
				{
					*backLen = n * 8;
				}

				if (n == 0)
				{
					n = 1;
				}

				if (n > MFRC522_MAX_LEN)
				{
					n = MFRC522_MAX_LEN;
				}

				for (i = 0; i < n; i++)
				{
					backData[i] = MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA);
				}
			}
		}

		else
		{
			status = MI_ERR;
		}
	}

	return status;
}

MFRC522_Status_t MFRC522_Anticoll(uint8_t* serNum)
{
	MFRC522_Status_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;

	MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);		//TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == MI_OK)
	{
		for (i = 0; i < 4; i++)
		{
			serNumCheck ^= serNum[i];
		}

		if (serNumCheck != serNum[i])
		{
			status = MI_ERR;
		}
	}
	return status;
}

void MFRC522_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData)
{
	uint8_t i, n;

	MFRC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);
	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
	//MFRC522_Write(CommandReg, PCD_IDLE);

	for (i = 0; i < len; i++)
	{
		 MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata+i));
	}

	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	i = 0xFF;

	do
	{
		n = MFRC522_ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	} while ((i!=0) && !(n&0x04));

	pOutData[0] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

uint8_t MFRC522_SelectTag(uint8_t* serNum)
{
	uint8_t i;
	MFRC522_Status_t status;
	uint8_t size;
	uint16_t recvBits;
	uint8_t buffer[9];

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;

	for (i = 0; i < 5; i++)
	{
		buffer[i+2] = *(serNum+i);
	}

	MFRC522_CalculateCRC(buffer, 7, &buffer[7]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18))
	{
		size = buffer[0];
	}

	else
	{
		size = 0;
	}

	return size;
}

MFRC522_Status_t  MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum)
{
	MFRC522_Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12];

	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++)
	{
		buff[i+2] = *(Sectorkey+i);
	}

	for (i=0; i<4; i++)
	{
		buff[i+8] = *(serNum+i);
	}

	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

	if ((status != MI_OK) || (!(MFRC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08)))
	{
		status = MI_ERR;
	}

	return status;
}

MFRC522_Status_t MFRC522_Read(uint8_t blockAddr, uint8_t* recvData)
 {
	MFRC522_Status_t status;
	uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	MFRC522_CalculateCRC(recvData,2, &recvData[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if ((status != MI_OK) || (unLen != 0x90))
	{
		status = MI_ERR;
	}

	return status;
}

MFRC522_Status_t MFRC522_Write(uint8_t blockAddr, uint8_t* writeData)
 {
	MFRC522_Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	MFRC522_CalculateCRC(buff, 2, &buff[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		for (i = 0; i < 16; i++)
		{
			buff[i] = *(writeData+i);
		}
		MFRC522_CalculateCRC(buff, 16, &buff[16]);
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {
			status = MI_ERR;
		}
	}

	return status;
}

void MFRC522_Halt(void)
{
	uint16_t unLen;
	uint8_t buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	MFRC522_CalculateCRC(buff, 2, &buff[2]);

	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

void MFRC522_Init(void)
{

	MFRC522_Reset();

	MFRC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);
	MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E);
	MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);
	MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);

	MFRC522_WriteRegister(MFRC522_REG_RF_CFG, 0x70);

	MFRC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40);
	MFRC522_WriteRegister(MFRC522_REG_MODE, 0x3D);

	MFRC522_AntennaOn();
}
