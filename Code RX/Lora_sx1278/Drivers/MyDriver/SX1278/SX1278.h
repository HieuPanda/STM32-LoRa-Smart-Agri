#ifndef SX1278_H
#define SX1278_H_H
#include "stm32f4xx_hal.h"

typedef struct {
    SPI_HandleTypeDef* hspi;         // Con trỏ tới SPI instance (SPI1, SPI2, ...)
    GPIO_TypeDef* Nss_Port;          // Port của chân NSS (CS)
    uint16_t Nss_Pin;                // Pin số của NSS
    GPIO_TypeDef* RESET_Port;        // Port của chân RESET
    uint16_t RESET_Pin;              // Pin số của RESET
    GPIO_TypeDef* DIO0_Port;         // Port của chân DIO0 (IRQ - TX_DONE/RX_DONE)
    uint16_t DIO0_Pin;               // Pin số của DIO0
} SX1278_t;



// === Core Configuration Registers ===
#define REG_FIFO                       0x00
#define REG_OP_MODE                    0x01
#define REG_FRF_MSB                    0x06
#define REG_FRF_MID                    0x07
#define REG_FRF_LSB                    0x08
#define REG_PA_CONFIG                  0x09
#define REG_LNA                        0x0C
#define REG_FIFO_ADDR_PTR              0x0D
#define REG_FIFO_TX_BASE_ADDR          0x0E
#define REG_FIFO_RX_BASE_ADDR          0x0F
#define REG_FIFO_RX_CURRENT_ADDR       0x10
#define REG_IRQ_FLAGS                  0x12
#define REG_RX_NB_BYTES                0x13
#define REG_PKT_SNR_VALUE              0x19
#define REG_PKT_RSSI_VALUE             0x1A
#define REG_MODEM_CONFIG_1             0x1D
#define REG_MODEM_CONFIG_2             0x1E
#define REG_SYMB_TIMEOUT_LSB           0x1F
#define REG_PREAMBLE_MSB               0x20
#define REG_PREAMBLE_LSB               0x21
#define REG_PAYLOAD_LENGTH             0x22
#define REG_MODEM_CONFIG_3             0x26
#define REG_FREQ_ERROR_MSB             0x28
#define REG_FREQ_ERROR_MID             0x29
#define REG_FREQ_ERROR_LSB             0x2A
#define REG_RSSI_WIDEBAND              0x2C

// === Device Version (optional, debug) ===
#define REG_VERSION                    0x42

// === Frequcey 433MHz
#define FREQ_433_MHZ_MSB 			   0x6C
#define FREQ_433_MHZ_MID			   0x80
#define FREQ_433_MHZ_LSB			   0x00


// === DIO Mapping Registers ===
#define REG_DIO_MAPPING_1              0x40
#define REG_DIO_MAPPING_2              0x41

// === Sync Word ===
#define REG_SYNC_WORD                  0x39

// Operation modes
#define MODE_LONG_RANGE_MODE           0x80  // Bit 7 = 1 → LoRa mode
#define MODE_SLEEP                     0x00
#define MODE_STDBY                     0x01
#define MODE_TX                        0x03
#define MODE_RX_CONTINUOUS             0x05
#define MODE_RX_SINGLE                 0x06

#define IRQ_RX_TIMEOUT                 0x80
#define IRQ_RX_DONE                    0x40
#define IRQ_PAYLOAD_CRC_ERROR          0x20
#define IRQ_VALID_HEADER               0x10
#define IRQ_TX_DONE                    0x08
#define IRQ_CAD_DONE                   0x04
#define IRQ_FHSS_CHANGE_CHANNEL        0x02
#define IRQ_CAD_DETECTED               0x01

#define IRQ_ALL                        0xFF

// Bandwidth (in RegModemConfig1)
#define BW_7_8_KHZ                     0x00
#define BW_10_4_KHZ                    0x10
#define BW_15_6_KHZ                    0x20
#define BW_20_8_KHZ                    0x30
#define BW_31_25_KHZ                   0x40
#define BW_41_7_KHZ                    0x50
#define BW_62_5_KHZ                    0x60
#define BW_125_KHZ                     0x70  // Default
#define BW_250_KHZ                     0x80
#define BW_500_KHZ                     0x90

// Spreading Factor (in RegModemConfig2)
#define SF_6                           0x60
#define SF_7                           0x70
#define SF_8                           0x80
#define SF_9                           0x90
#define SF_10                          0xA0
#define SF_11                          0xB0
#define SF_12                          0xC0

// Coding Rate (in RegModemConfig1)
#define CR_4_5                         0x02
#define CR_4_6                         0x04
#define CR_4_7                         0x06
#define CR_4_8                         0x08

// PA config (RegPaConfig)
#define PA_BOOST                       0x80 // Output on PA_BOOST pin
#define PA_MAX_POWER                   0x70 // Max power setting
#define PA_OUTPUT_POWER(x)             (x & 0x0F) // 0 to 15

#define LOW_DATA_RATE_OPTIMIZE_OFF     0x04
#define LNA_MAX_GAIN				   0x23

uint8_t SX1278_ReadRegister(SX1278_t* _sx1278 , uint8_t reg);
void SX1278_Init(SX1278_t* _sx1278);
void SX1278_InitTx(SX1278_t* _sx1278);
void SX1278_InitRx(SX1278_t* _sx1278);
void SX1278_Transmit(SX1278_t* _sx1278, uint8_t *data, uint8_t len);
uint8_t SX1278_Receive(SX1278_t* _sx1278, uint8_t *data_receive);
void SX1278_WriteRegister(SX1278_t* _sx1278 , uint8_t reg, uint8_t value);

#endif
