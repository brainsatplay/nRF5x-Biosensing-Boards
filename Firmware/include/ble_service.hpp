#pragma once

#include <functional>

namespace Bluetooth
{
    /**
     * @brief Function used to setup BLE Service
     * 
     * @return BLE error code 
     */
    int SetupBLE();

    /**
     * @brief Send BLE notification through ADS131M08 Data Pipe.
     * 
     * @param data pointer to datasource containing ADS131M08 data samples
     * @param len  the number of samples to transfer
     */
    void Ads131m08Notify(const uint8_t* data, const uint8_t len);

    /**
     * @brief Send BLE notification through ADS131M08_1 Data Pipe.
     * 
     * @param data pointer to datasource containing ADS131M08 data samples
     * @param len  the number of samples to transfer
     */
    void Ads131m08_1_Notify(const uint8_t* data, const uint8_t len);

    /**
     * @brief Send BLE notification through MAX30102 Data Pipe.
     * 
     * @param data pointer to datasource containing MAX30102 data samples
     * @param len  the number of samples to transfer
     */
    void Max30102Notify(const uint8_t* data, const uint8_t len);

    /**
     * @brief Send BLE notification through MPU6050 Data Pipe.
     * 
     * @param data pointer to datasource containing MPU6050 data samples
     * @param len  the number of samples to transfer
     */
    void Mpu6050Notify(const uint8_t* data, const uint8_t len);
}
