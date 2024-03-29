/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>
#include "hardware/i2c.h"
#include <functional>

namespace cilo72
{
    namespace hw
    {
        /**
         * @brief The I2CBus class provides an interface for communication with devices through the I2C bus.
         *
         * This class allows to write data to a device, read data from a device or transfer data to and from a device.
         */
        class I2CBus
        {
        public:
            /**
             * @brief Constructor for the I2CBus class.
             * @param pin_i2c_sda The SDA pin for the I2C bus.
             * @param pin_i2c_scl The SCL pin for the I2C bus.
             */
            I2CBus(uint pin_i2c_sda, uint pin_i2c_scl);

            /**
             * @brief Writes data to a device.
             * @param addr The address of the device.
             * @param src A pointer to the source data to be written.
             * @param len The number of bytes to write, must be one ore more.
             * @param nostop If true, the stop bit is not set after the write operation.
             * @return True if the write operation was successful, false otherwise.
             * @note This function is blocking.
             */
            bool writeBlocking(uint8_t addr, const uint8_t *src, size_t len, bool nostop = false) const;


            /**
             * @brief Writes data to a device.
             * @param addr The address of the device.
             * @param data A function that provides the data to be written.
             * @return True if the write operation was successful, false otherwise.
             * @note This function is blocking.
             * 
             * index is the index of the byte to be written, byte is the byte to be written.
             * The function must return true if more data has to be written, false otherwise.
             */
            bool writeBlocking(uint8_t addr, std::function<bool(size_t index, uint8_t & byte)> data) const;

            /**
             * @brief Reads data from a device.
             * @param addr The address of the device.
             * @param dst A pointer to the destination buffer for the read data.
             * @param len The number of bytes to read.
             * @return True if the read operation was successful, false otherwise.
             */
            bool readBlocking(uint8_t addr, uint8_t *dst, size_t len) const;

            /**
             * @brief Transfers data to and from a device.
             * @param addr The address of the device.
             * @param src A pointer to the source data to be written.
             * @param lenSrc The number of bytes to write.
             * @param dst A pointer to the destination buffer for the read data.
             * @param lenDst The number of bytes to read.
             * @return True if the transfer operation was successful, false otherwise.
             */
            bool xfer(uint8_t addr, const uint8_t *src, size_t lenSrc, uint8_t *dst, size_t lenDst) const;

        private:
            i2c_inst_t *i2cInstance_; ///< Pointer to the underlying I2C instance.
        };

    }
}
