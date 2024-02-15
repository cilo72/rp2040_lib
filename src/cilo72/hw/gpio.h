/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include "pico/stdlib.h"

namespace cilo72
{
    namespace hw
    {
        class Gpio
        {
        public:
            enum class Direction
            {
                Output,
                Input
            };

            enum class Level
            {
                Low,
                High
            };

            enum class Pull
            {
                None,
                Up,
                Down
            };

            Gpio(uint pin, Direction direction, Pull pull, Level level = Level::Low)
            : pin_(pin)
            {
                gpio_init(pin);
                switch (direction)
                {
                case Direction::Output:
                    gpio_put(pin, level == Level::High);
                    gpio_set_dir(pin, GPIO_OUT);
                    break;

                case Direction::Input:
                    gpio_set_dir(pin, GPIO_IN);
                    switch (pull)
                    {
                    case Pull::None:
                        gpio_set_pulls(pin, false, false);
                        break;

                    case Pull::Up:
                        gpio_set_pulls(pin, true, false);
                        break;

                    case Pull::Down:
                        gpio_set_pulls(pin, false, true);
                        break;
                    }
                    break;
                }
            }

            Gpio(uint pin, Direction direction, Level level = Level::Low)
                : Gpio(pin, direction, Pull::None, level)
            {
            }

            void set(Level level) const
            {
                gpio_put(pin_, level == Level::High);
            }

            void set() const
            {
                gpio_put(pin_, true);
            }

            void clear() const
            {
                gpio_put(pin_, false);
            }

            void toggle() const
            {
                gpio_put(pin_, not gpio_get_out_level(pin_));
            }

            bool get() const
            {
                return gpio_get(pin_);
            }

            bool isHigh() const
            {
                return gpio_get_out_level(pin_);
            }

        private:
            uint pin_;
        };
    }
}