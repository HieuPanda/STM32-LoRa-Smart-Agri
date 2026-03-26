# STM32-LoRa-Smart-Agri
📋 Overview
This project implements a Master-Slave architecture to monitor temperature and humidity in agricultural environments. The system uses LoRa technology for long-range wireless communication and features automated actuators (pump/fan) with intelligent non-blocking control logic.

🚀 Key Features
Long-Range Communication: Reliable data transmission using SX1278 LoRa modules.

High Precision Sensing: Real-time temperature and humidity tracking via AHT10 sensor (I2C).

Data Integrity: Custom Checksum (Summation) verification to ensure error-free packet delivery.

Smart Automation: * Automatic fan activation for high-temperature cooling.

Timed-pulse irrigation (Pump) based on humidity thresholds.

Non-blocking logic using HAL_GetTick() to maintain system responsiveness.

Optimized Payload: Compact 6-byte binary packet structure for maximum transmission efficiency.

🛠 Hardware Requirements
Microcontroller: STM32F407VGT6 (Discovery Board)

Wireless Module: Semtech SX1278 LoRa (433MHz)

Sensors: AHT10 (Temperature & Humidity)

Actuators: 5V/12V Relay Modules for Pump and Fan

📂 Project Structure
/Core/Src: Main application logic and HAL initialization.

/Drivers/MyDriver: Custom drivers for SX1278 and AHT10.

SensorPacket: Custom packed struct for efficient data serialization.

🔧 Installation & Usage
Clone the repository: git clone https://github.com/HieuPanda/STM32-LoRa-Smart-Agri.git

Open the project in STM32CubeIDE.

Configure SPI for SX1278 and I2C for AHT10.

Build and Flash the Master code to the sensor node.

Build and Flash the Slave code to the controller node.
