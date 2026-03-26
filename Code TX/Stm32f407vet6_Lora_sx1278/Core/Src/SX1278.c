#include "SX1278.h"

uint8_t SX1278_ReadRegister(SX1278_t* _sx1278 , uint8_t reg)
{
//    uint8_t tx = reg & 0x7F;  // ✅ Đặt bit 7 = 1 để báo là Read
    uint8_t rx = 0;

    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_sx1278->hspi, &reg, 1 ,1000);
    HAL_SPI_Receive(_sx1278->hspi, &rx, 1 ,1000);
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_SET);

    return rx;
}

void SX1278_WriteRegister(SX1278_t* _sx1278 , uint8_t reg, uint8_t value)
{
    uint8_t buff[2] = { reg | 0x80, value }; //reg | 0x80 reg & 0x7F;
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_sx1278->hspi, buff, 2 ,1000);
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_SET);
}

void SX1278_Reset(SX1278_t* _sx1278)
{
    HAL_GPIO_WritePin(_sx1278->RESET_Port, _sx1278->RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(100); // Đảm bảo bạn có định nghĩa hàm Delay_us
    HAL_GPIO_WritePin(_sx1278->RESET_Port, _sx1278->RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}


void SX1278_InitTx(SX1278_t* _sx1278)
{
    SX1278_Reset(_sx1278);
    HAL_Delay(10);

    // Bắt buộc: Vào Sleep trước để đảm bảo mode chuyển được
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_SLEEP);
    HAL_Delay(10);

    // Bật LoRa mode trong Sleep
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
    HAL_Delay(10);

    // Vào Standby để bắt đầu config
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    HAL_Delay(10);

    // Tiếp tục các config như thường
    SX1278_WriteRegister(_sx1278, REG_LNA, LNA_MAX_GAIN);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_1, BW_125_KHZ | CR_4_5);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_2, SF_7 | 0x04 );
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_3, LOW_DATA_RATE_OPTIMIZE_OFF);

    SX1278_WriteRegister(_sx1278, REG_FRF_MSB, FREQ_433_MHZ_MSB);
    SX1278_WriteRegister(_sx1278, REG_FRF_MID, FREQ_433_MHZ_MID);
    SX1278_WriteRegister(_sx1278, REG_FRF_LSB, FREQ_433_MHZ_LSB);

    SX1278_WriteRegister(_sx1278, REG_PA_CONFIG, PA_MAX_POWER);
		SX1278_WriteRegister(_sx1278, REG_PA_CONFIG, 0x8F); // PA_BOOST | MaxPower | OutputPower = 15
    SX1278_WriteRegister(_sx1278, REG_FIFO_TX_BASE_ADDR, 0x00);
    SX1278_WriteRegister(_sx1278, REG_SYNC_WORD, 0x34);

}

void SX1278_InitRx(SX1278_t* _sx1278)
{
    SX1278_Reset(_sx1278);
    HAL_Delay(10);

    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

    SX1278_WriteRegister(_sx1278, REG_LNA, LNA_MAX_GAIN);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_1, BW_125_KHZ | CR_4_5);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_2, SF_7 | 0x04);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_3, LOW_DATA_RATE_OPTIMIZE_OFF);

    SX1278_WriteRegister(_sx1278, REG_FRF_MSB, FREQ_433_MHZ_MSB);
    SX1278_WriteRegister(_sx1278, REG_FRF_MID, FREQ_433_MHZ_MID);
    SX1278_WriteRegister(_sx1278, REG_FRF_LSB, FREQ_433_MHZ_LSB);

    SX1278_WriteRegister(_sx1278, REG_FIFO_RX_BASE_ADDR, 0x00);
    SX1278_WriteRegister(_sx1278, REG_SYNC_WORD, 0x34); // Giống TX

    // Bắt đầu chế độ nhận
    SX1278_WriteRegister(_sx1278, REG_IRQ_FLAGS, IRQ_ALL); // Xóa flag cũ
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

void SX1278_Init(SX1278_t* _sx1278)
{

    SX1278_Reset(_sx1278);
    HAL_Delay(10);
    uint8_t check =  SX1278_ReadRegister(_sx1278, 0x42);
		if(check == 0x12 )
	{
		 HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_6);
	}
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE);
    HAL_Delay(10);
    uint8_t checkMode =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
	if(checkMode == MODE_LONG_RANGE_MODE )
	{
		 HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_6);
	}
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    HAL_Delay(10);
    uint8_t op = SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    SX1278_WriteRegister(_sx1278, REG_LNA, LNA_MAX_GAIN);
    HAL_Delay(10);
    uint8_t checkLNA = SX1278_ReadRegister( _sx1278, REG_LNA);
			if (checkLNA == LNA_MAX_GAIN)
	{
		 HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_6);
	}
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_1, BW_125_KHZ | CR_4_5);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_2, SF_7 | 0x04);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_3, LOW_DATA_RATE_OPTIMIZE_OFF);
     check =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    SX1278_WriteRegister(_sx1278, REG_FRF_MSB, FREQ_433_MHZ_MSB);
    SX1278_WriteRegister(_sx1278, REG_FRF_MID, FREQ_433_MHZ_MID);
	    uint8_t checkmid = SX1278_ReadRegister( _sx1278, REG_FRF_MID);
			if (checkmid == FREQ_433_MHZ_MID)
	{
		 HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_6);
	}
    SX1278_WriteRegister(_sx1278, REG_FRF_LSB, FREQ_433_MHZ_LSB);

    SX1278_WriteRegister(_sx1278, REG_PA_CONFIG, PA_MAX_POWER);
    SX1278_WriteRegister(_sx1278, REG_FIFO_TX_BASE_ADDR, 0x00);
    SX1278_WriteRegister(_sx1278, REG_FIFO_RX_BASE_ADDR, 0x00);
	  //SX1278_WriteRegister(_sx1278, REG_DIO_MAPPING_1, 0x00);  // DIO0 = RxDone, DIO0 sẽ
	
	// Thêm dòng này:
	SX1278_WriteRegister(_sx1278, REG_SYNC_WORD, 0x34);
	SX1278_WriteRegister(_sx1278, REG_DIO_MAPPING_1, 0x00);
}

void SX1278_Transmit(SX1278_t* _sx1278, uint8_t *data, uint8_t len)
{
    // 1. Vào Sleep Mode + LoRa để reset state máy
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
    HAL_Delay(10);

    // 2. Vào Standby Mode để chuẩn bị ghi dữ liệu
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    HAL_Delay(10);

    // 3. Ghi địa chỉ FIFO bắt đầu từ TX base (0x00)
    SX1278_WriteRegister(_sx1278, REG_FIFO_ADDR_PTR, 0x00);

    // 4. Ghi dữ liệu vào FIFO qua SPI
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_RESET);
    uint8_t fifo_cmd = REG_FIFO | 0x80;  // FIFO write
    HAL_SPI_Transmit(_sx1278->hspi, &fifo_cmd, 1, 1000);
    HAL_SPI_Transmit(_sx1278->hspi, data, len, 1000);
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_SET);

    // 5. Ghi độ dài payload
    SX1278_WriteRegister(_sx1278, REG_PAYLOAD_LENGTH, len);

    // 6. Vào chế độ phát
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

    // 7. Đợi khi có cờ TX_DONE (có thể dùng polling hoặc interrupt)
    while ((SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS) & IRQ_TX_DONE) == 0);

    // 8. Xoá tất cả cờ
    SX1278_WriteRegister(_sx1278, REG_IRQ_FLAGS, IRQ_ALL);
}

uint8_t SX1278_Receive(SX1278_t* _sx1278, uint8_t *data_receive)
{
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
		    uint8_t checkMode =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
	if(checkMode == 0x85 )
	{
		 HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_6);
	}
    while ((SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS) & IRQ_RX_DONE) == 0);

    uint8_t irqFlags = SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS);
    if (irqFlags & IRQ_PAYLOAD_CRC_ERROR) {
        SX1278_WriteRegister(_sx1278, REG_IRQ_FLAGS, irqFlags);
        return 0;
    }

    uint8_t len = SX1278_ReadRegister(_sx1278, REG_RX_NB_BYTES);
    uint8_t fifoAddr = SX1278_ReadRegister(_sx1278, REG_FIFO_RX_CURRENT_ADDR);
    SX1278_WriteRegister(_sx1278, REG_FIFO_ADDR_PTR, fifoAddr);

    uint8_t addr = 0x00;
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_sx1278->hspi, &addr, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(_sx1278->hspi, data_receive, len, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_SET);

    SX1278_WriteRegister(_sx1278, REG_IRQ_FLAGS, IRQ_RX_DONE | IRQ_PAYLOAD_CRC_ERROR);

    return len;
}
