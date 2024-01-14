/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>
#include <functional>
#include "pico/stdlib.h"
#include "cilo72/hw/spi_bus.h"

namespace cilo72
{
    namespace hw
    {
        /**
         * @brief The SPIDevice class provides an interface to an SPI device.
         */
        class SPIDevice
        {
        public:
            /**
             * @brief Constructs an SPIDevice object with the given SPI bus, CSN pin, baudrate, data bits, clock polarity and clock phase.
             * @param spiBus The SPIBus object representing the SPI bus.
             * @param pin_spi_csn The CSN (chip select not) pin number.
             * @param baudrate The baudrate in Hz.
             * @param data_bits The number of data bits per transfer.
             * @param cpol The clock polarity.
             * @param cpha The clock phase.
             */
            SPIDevice(SPIBus &spiBus, uint8_t pin_spi_csn, uint baudrate = 1000000, uint data_bits = 8, spi_cpol_t cpol = SPI_CPOL_1, spi_cpha_t cpha = SPI_CPHA_1);

            /**
             * @brief Transfers data over the SPI bus.
             * @param tx The data to transmit.
             * @param rx The buffer to receive the data.
             * @param len The length of the data.
             */
            void xfer(const uint8_t *tx, uint8_t *rx, size_t len) const;

            /**
             * @brief Writes data to the SPI bus.
             * @param tx The data to transmit.
             * @param len The length of the data.
             * @param repeat The number of times to repeat the data.
             */
            void write(const uint8_t *tx, size_t len, uint32_t repeat = 1) const;

            /**
             * @brief Set data format
             * @param data_bits Number of data bits per transfer
             * @param cpol Clock polarity
             * @param cpha Clock phase
             */
            void setFormat(uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha);

            /**
             * @brief Set baudrate
             * @param baudrate Baudrate in Hz
             */
            void setBaudrate(uint baudrate);
        private:
            SPIBus &spiBus_;
            uint8_t pin_spi_csn_;
            uint baudrate_;
            uint data_bits_;
            spi_cpol_t cpol_;
            spi_cpha_t cpha_;
            void csSelect() const;
            void csDeselect() const;
            void config();
        };
    }
}
