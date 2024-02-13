/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/hw/adc.h"
#include <cstring>

namespace cilo72
{
    namespace hw
    {
        namespace
        {
            bool initialized = false;
        }

        Adc::Adc(uint pin)
        {
            if(!initialized)
            {
                adc_init();
                initialized = true;
            }
            switch(pin)
            {
                case 26:
                    input_ = 0;
                    break;
                case 27:
                    input_ = 1;
                    break;
                case 28:
                    input_ = 2;
                    break;
                case 29:
                    input_ = 3;
                    break;  
                default:
                    assert(0);
                    break;
            }
            adc_gpio_init(pin);
        }

            uint32_t Adc::read() const
            {
                adc_select_input(input_);
                uint16_t adc = adc_read();
                return (adc * maxVoltage()) / maxCount();
            }

            uint32_t Adc::readRaw() const
            {
                adc_select_input(input_);
                return adc_read();
            }

            uint32_t Adc::maxVoltage() const
            {
                return 3300;
            }

            uint32_t Adc::maxCount() const
            {
                return 4095;
            }
    }
}
