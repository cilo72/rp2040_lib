/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/hw/gpiokey.h"

namespace cilo72
{
    namespace hw
    {
        GpioKey::GpioKey(uint8_t pinKey, Gpio::Pull pull = Gpio::Pull::None)
            : repeatingTimer_(100, [&]()
                              { GpioKey::timerCallback(); }),
                              gpio_(pinKey, Gpio::Direction::Input, pull),
                              pressed_(false)

        {
            down_ = lastDown_ = debounceSW_.in(gpio_.get());
        }

        void GpioKey::timerCallback()
        {
            down_ = not debounceSW_.in(gpio_.get());

            if(lastDown_ == false and down_ == true)
            {
                pressed_ = true;
            }

            lastDown_ = down_;
        }

        bool GpioKey::isPressed() const
        {
            return down_;
        }

        bool GpioKey::pressed()
        {
          bool ret = pressed_;
          pressed_ = false;
          return ret;
        }

    }
}
