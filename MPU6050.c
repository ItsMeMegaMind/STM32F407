#include <STM32F4XX.H>
#include <STM32F4XX_RCC.H>
#include <STM32F4XX_I2C.H>
#include <STM32F4XX_GPIO.H>

#define MPU6050_I2C_ADDR			0xD0

#define MPU6050_I_AM				0x68

#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75

#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)
#define MPU6050_GYRO_SENS_2000		((float) 16.4)

#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)

__IO uint32_t TimingDelay;

uint8_t MPU6050_Data_Buffer[14];
int16_t Acc_X, Acc_Y, Acc_Z, Gyr_X, Gyr_Y, Gyr_Z, Temp;

void MPU6050_GPIO_Configure()
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

void MPU6050_I2C_Configure()
{
	I2C_InitTypeDef I2C_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitStructure);
    I2C_Cmd(I2C2, ENABLE);
}

void MPU6050_Write_Data(uint8_t Address, uint8_t Data)
{
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

	I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, MPU6050_I2C_ADDR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, Address);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C2, Data);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C2, ENABLE);
}


void MPU6050_Read_Data()
{
	uint8_t i;

	I2C_AcknowledgeConfig(I2C2, ENABLE);

    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, MPU6050_I2C_ADDR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, MPU6050_ACCEL_XOUT_H);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, MPU6050_I2C_ADDR, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    for (i=0;i<14;i++)
    {
    	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
        MPU6050_Data_Buffer[i] = I2C_ReceiveData(I2C2);

        if (i == 12)
        {
        	I2C_NACKPositionConfig(I2C2, I2C_NACKPosition_Current);
            I2C_AcknowledgeConfig(I2C2, DISABLE);
        }
    }

    Acc_X = MPU6050_Data_Buffer[0]<<8 | MPU6050_Data_Buffer[1];
    Acc_Y = MPU6050_Data_Buffer[2]<<8 | MPU6050_Data_Buffer[3];
    Acc_Z = MPU6050_Data_Buffer[4]<<8 | MPU6050_Data_Buffer[5];

    Gyr_X = MPU6050_Data_Buffer[8]<<8  | MPU6050_Data_Buffer[9];
    Gyr_Y = MPU6050_Data_Buffer[10]<<8 | MPU6050_Data_Buffer[11];
    Gyr_Z = MPU6050_Data_Buffer[12]<<8 | MPU6050_Data_Buffer[13];

    Temp = MPU6050_Data_Buffer[6]<<8 | MPU6050_Data_Buffer[7];

    Temp = (float)((float)((int16_t)Temp/(float)340.0) + (float)36.53);
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

int main()
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000);

	Delay(100);
	MPU6050_GPIO_Configure();
	MPU6050_I2C_Configure();

	Delay(100);
	MPU6050_Write_Data(MPU6050_PWR_MGMT_1, 0x00);

	while(1)
	{
		MPU6050_Read_Data();
		Delay(100);
	}
}
