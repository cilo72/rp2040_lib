/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

/**
 * @file
 * @brief This header file contains the declaration of the SPIBus class.
 */

#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

namespace cilo72
{
    namespace hw
    {
        /**
         * @brief The SPIBus class provides an interface to an SPI bus.
         */
        class SPIBus
        {
        public:
            static constexpr uint8_t PIN_NOT_USED = 255; /**< The pin number indicating that the pin is not used. */

            /**
             * @brief Constructs an SPIBus object with the given SCK, and TX pins.
             * The constructor selects the SPI instance according the pins. If the instance is already used, an assertion occurs.
             * @param pin_spi_sck The SCK pin number.
             * @param pin_spi_tx The TX pin number.
             */
            SPIBus(uint8_t pin_spi_sck, uint8_t pin_spi_tx);

            /**
             * @brief Constructs an SPIBus object with the given SCK, RX, and TX pins.
             * The constructor selects the SPI instance according the pins. If the instance is already used, an assertion occurs.
             * @param pin_spi_sck The SCK pin number.
             * @param pin_spi_rx The RX pin number.
             * @param pin_spi_tx The TX pin number.
             */
            SPIBus(uint8_t pin_spi_sck, uint8_t pin_spi_rx, uint8_t pin_spi_tx);

            /**
             * @brief Gets the SPI instance used by this SPIBus object.
             * @return A pointer to the spi_inst_t struct representing the SPI instance.
             */
            spi_inst_t *instance() const { return spiInstance_; };

            void config(uint baudrate, uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha);

        private:
            spi_inst_t *spiInstance_; /**< The SPI instance used by this SPIBus object. */
            uint baudrate_;
            uint data_bits_;
            spi_cpol_t cpol_;
            spi_cpha_t cpha_;
        };
    }
}
