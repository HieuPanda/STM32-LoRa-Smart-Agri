#ifndef AHT10_H
#define AHT10_H


void AHT10_Init(void);
void AHT10_SoftReset(void);
uint8_t AHT10_ReadData(float *humidity, float *temperature);


// Các comand
#define AHT10_ADDRESS     0x38<<1
#define AHT10_CMD_INIT    0XE1
#define AHT10_CMD_MEASURE 0xAC
#define AHT10_CMD_RESET   0xBA

#endif
