/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>
#include <functional>
#include "pico/stdlib.h"
#include "hardware/adc.h"

namespace cilo72
{
    namespace hw
    {
        /**
         * @brief Class to read the analog to digital converter
         */
        class Adc
        {
        public:
            /**
             * @brief Construct a new Adc object
             * 
             * @param pin The pin to read the analog value from
             */
            Adc(uint pin);

            /**
             * @brief Read the analog value
             * 
             * @return uint32_t The volatge in mV. The value is between 0 and 3300
             */
            uint32_t read() const;

            /**
             * @brief Read the raw value
             * 
             * @return uint32_t The raw value. The value is between 0 and 4095
             */
            uint32_t readRaw() const;

            /**
             * @brief Returns the maximum voltage in mV.
             */
            uint32_t maxVoltage() const;

            /**
             * @brief Returns the maximum count of the ADC
             */
            uint32_t maxCount() const;
        private:
            uint input_;
        };
    }
}
