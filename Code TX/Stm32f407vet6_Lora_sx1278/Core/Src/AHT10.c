#include "stm32f4xx_hal.h"
#include "AHT10.h"

extern I2C_HandleTypeDef hi2c2;
#define AHT10_I2C &hi2c2


void AHT10_Init(void)
{
    uint8_t init_cmd[] ={AHT10_CMD_INIT, 0x80, 0x00};
	HAL_I2C_Master_Transmit(AHT10_I2C, AHT10_ADDRESS, init_cmd, 3, 100);
	HAL_Delay(10); 
}


void AHT10_TrigerMeasure(void)
{
	uint8_t triger_cmd[]= {AHT10_CMD_MEASURE, 0x33, 0x00};
	HAL_I2C_Master_Transmit(AHT10_I2C, AHT10_ADDRESS, triger_cmd, 3, 1000);
	HAL_Delay(80);
}

void AHT10_SoftReset(void)
{
    uint8_t softReCmd = AHT10_CMD_RESET;
	HAL_I2C_Master_Transmit(AHT10_I2C, AHT10_ADDRESS, &softReCmd, 1, 1000);
	HAL_Delay(20);
}

uint8_t AHT10_ReadData(float *humidity, float *temperature)
{
	uint8_t data[6];
	uint32_t raw_humi;
	uint32_t raw_temp;
	AHT10_TrigerMeasure();
	
	HAL_I2C_Master_Receive(AHT10_I2C, AHT10_ADDRESS, data, 6 , 1000);
	
	if((data[0] & 0x80) == 0x00)
	{
		raw_humi = ((uint32_t)data[1] <<12) |
				   ((uint32_t)data[2] <<4)  |
				   ((data[3] & 0xF0 )>>4);
		raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) |
		           ((uint32_t)data[4] << 8) |
		           ((uint32_t)data[5]);
					   
	*temperature = ((float)raw_temp*200.0) / (1048576.0) - 50.0;
	*humidity = ((float)raw_humi*100.0)/(1048576.0);
	return 0;
	}
	return 1;
}
