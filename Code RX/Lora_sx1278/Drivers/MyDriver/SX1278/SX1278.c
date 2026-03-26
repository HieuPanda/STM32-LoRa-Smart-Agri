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

    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

    SX1278_WriteRegister(_sx1278, REG_LNA, LNA_MAX_GAIN);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_1, BW_125_KHZ | CR_4_5);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_2, SF_7 | 0x04);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_3, LOW_DATA_RATE_OPTIMIZE_OFF);

    SX1278_WriteRegister(_sx1278, REG_FRF_MSB, FREQ_433_MHZ_MSB);
    SX1278_WriteRegister(_sx1278, REG_FRF_MID, FREQ_433_MHZ_MID);
    SX1278_WriteRegister(_sx1278, REG_FRF_LSB, FREQ_433_MHZ_LSB);

    SX1278_WriteRegister(_sx1278, REG_PA_CONFIG, PA_MAX_POWER);
    SX1278_WriteRegister(_sx1278, REG_FIFO_TX_BASE_ADDR, 0x00);
    SX1278_WriteRegister(_sx1278, REG_SYNC_WORD, 0x34); // Phải giống RX
}

void SX1278_InitRx(SX1278_t* _sx1278)
{
    // Reset chip
    SX1278_Reset(_sx1278);
    HAL_Delay(10);

    // Bắt buộc: Vào LoRa mode (Sleep trước rồi Standby)
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, 0x80); // LoRa + Sleep
    HAL_Delay(10);
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, 0x81); // LoRa + Standby
    HAL_Delay(10);

    // Cấu hình LNA và modem
    SX1278_WriteRegister(_sx1278, REG_LNA, LNA_MAX_GAIN);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_1, BW_125_KHZ | CR_4_5);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_2, SF_7 | 0x04);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_3, LOW_DATA_RATE_OPTIMIZE_OFF);

    // Tần số
    SX1278_WriteRegister(_sx1278, REG_FRF_MSB, FREQ_433_MHZ_MSB);
    SX1278_WriteRegister(_sx1278, REG_FRF_MID, FREQ_433_MHZ_MID);
    SX1278_WriteRegister(_sx1278, REG_FRF_LSB, FREQ_433_MHZ_LSB);

    // Địa chỉ FIFO
    SX1278_WriteRegister(_sx1278, REG_FIFO_RX_BASE_ADDR, 0x00);
    SX1278_WriteRegister(_sx1278, REG_FIFO_ADDR_PTR, 0x00); // Đảm bảo con trỏ FIFO cũng set

    SX1278_WriteRegister(_sx1278, REG_PAYLOAD_LENGTH, 0xFF);
    // Từ hóa đồng bộ (sync word)
    SX1278_WriteRegister(_sx1278, REG_SYNC_WORD, 0x34); // Giống bên TX

    // Xóa tất cả interrupt cũ
    SX1278_WriteRegister(_sx1278, REG_IRQ_FLAGS, IRQ_ALL);

    // Đặt chế độ nhận liên tục
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
    HAL_Delay(10);
}

void SX1278_Init(SX1278_t* _sx1278)
{

    SX1278_Reset(_sx1278);
    HAL_Delay(10);
    uint8_t check =  SX1278_ReadRegister(_sx1278, 0x42);
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE);
    HAL_Delay(10);
    uint8_t checkMode =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    HAL_Delay(10);
    uint8_t op = SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    SX1278_WriteRegister(_sx1278, REG_LNA, LNA_MAX_GAIN);
    HAL_Delay(10);
    uint8_t checkLNA = SX1278_ReadRegister( _sx1278, REG_LNA);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_1, BW_125_KHZ | CR_4_5);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_2, SF_7 | 0x04);
    SX1278_WriteRegister(_sx1278, REG_MODEM_CONFIG_3, LOW_DATA_RATE_OPTIMIZE_OFF);
     check =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    SX1278_WriteRegister(_sx1278, REG_FRF_MSB, FREQ_433_MHZ_MSB);
    SX1278_WriteRegister(_sx1278, REG_FRF_MID, FREQ_433_MHZ_MID);
    SX1278_WriteRegister(_sx1278, REG_FRF_LSB, FREQ_433_MHZ_LSB);

    SX1278_WriteRegister(_sx1278, REG_PA_CONFIG, PA_MAX_POWER);
    SX1278_WriteRegister(_sx1278, REG_FIFO_TX_BASE_ADDR, 0x00);
    SX1278_WriteRegister(_sx1278, REG_FIFO_RX_BASE_ADDR, 0x00);

    SX1278_WriteRegister(_sx1278, REG_DIO_MAPPING_1, 0x00);
    // Thêm dòng này:
    SX1278_WriteRegister(_sx1278, REG_SYNC_WORD, 0x34);
}

void SX1278_Transmit(SX1278_t* _sx1278, uint8_t *data, uint8_t len)
{
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, 0x83);
    HAL_Delay(10);
    uint8_t check =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    SX1278_WriteRegister(_sx1278, REG_FIFO_ADDR_PTR, 0x00);
    SX1278_WriteRegister(_sx1278, REG_PAYLOAD_LENGTH, len);
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_RESET);
    uint8_t fifo_addr = 0x80;  // FIFO write command
    HAL_SPI_Transmit(_sx1278->hspi, &fifo_addr, 1, 1000);
    HAL_SPI_Transmit(_sx1278->hspi, data, len, 1000);
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_SET);

    //
    uint8_t fifo_read_addr = 0x00;  // FIFO read address
    uint8_t fifo_read_data[16] = {0}; // Dùng buffer đủ lớn

    SX1278_WriteRegister(_sx1278, REG_FIFO_ADDR_PTR, 0x00);  // Reset pointer về đầu FIFO

    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_sx1278->hspi, &fifo_read_addr, 1, 1000);  // FIFO read (bit 7 = 0)
    HAL_SPI_Receive(_sx1278->hspi, fifo_read_data, len, 1000);  // Đọc lại dữ liệu
    HAL_GPIO_WritePin(_sx1278->Nss_Port, _sx1278->Nss_Pin, GPIO_PIN_SET);
    //
  //  SX1278_WriteRegister(_sx1278, REG_PAYLOAD_LENGTH, len);
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
    HAL_Delay(5);
    check =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    while ((SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS) & IRQ_TX_DONE) == 0);

    HAL_Delay(5); // Cho chip kịp xử lý
    SX1278_WriteRegister(_sx1278, REG_IRQ_FLAGS, IRQ_ALL);
    check = SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS);
}

uint8_t SX1278_Receive(SX1278_t* _sx1278, uint8_t *data_receive)
{
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    SX1278_WriteRegister(_sx1278, REG_IRQ_FLAGS,IRQ_PAYLOAD_CRC_ERROR );
    SX1278_WriteRegister(_sx1278, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
//    SX1278_WriteRegister(_sx1278, REG_DIO_MAPPING_1, 0x00); // Map DIO0 → RxDone
    HAL_Delay(5);
    uint8_t check =  SX1278_ReadRegister(_sx1278, REG_OP_MODE);
    uint8_t checkconfig1 =  SX1278_ReadRegister(_sx1278, REG_MODEM_CONFIG_1);
    uint8_t checkconfig2 =  SX1278_ReadRegister(_sx1278, REG_MODEM_CONFIG_2);
    uint8_t checkconfig3 =  SX1278_ReadRegister(_sx1278, REG_MODEM_CONFIG_3);
    uint8_t checkfrfmsb =  SX1278_ReadRegister(_sx1278, REG_FRF_MSB);
    uint8_t checkfrfmid =  SX1278_ReadRegister(_sx1278, REG_FRF_MID);
    uint8_t checkfrflsb =  SX1278_ReadRegister(_sx1278, REG_FRF_LSB);
    uint8_t checkpa =  SX1278_ReadRegister(_sx1278, REG_PA_CONFIG);
    uint8_t checkfifotx =  SX1278_ReadRegister(_sx1278, REG_FIFO_TX_BASE_ADDR);
    uint8_t checkfiforx =  SX1278_ReadRegister(_sx1278, REG_FIFO_RX_BASE_ADDR);
    uint8_t checksync =  SX1278_ReadRegister(_sx1278, REG_SYNC_WORD);
    uint8_t rssi = SX1278_ReadRegister(_sx1278, 0x11);
    uint8_t irq = SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS);
    uint8_t current_addr = SX1278_ReadRegister(_sx1278, 0x1D);
    uint8_t byte_count = SX1278_ReadRegister(_sx1278, REG_RX_NB_BYTES);

    // Đợi đến khi có dữ liệu hoặc timeout
    uint32_t tickstart = HAL_GetTick();
    while ((SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS) & IRQ_RX_DONE) == 0) {
        if (HAL_GetTick() - tickstart > 1000) {
            return 0; // Timeout
        }
    }

    uint8_t irqFlags = SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS);
    if (irqFlags & IRQ_PAYLOAD_CRC_ERROR) {
    	uint8_t check =  SX1278_ReadRegister(_sx1278, REG_IRQ_FLAGS);
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
