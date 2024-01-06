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

            Gpio(uint pin, Direction direction, Level level = Level::Low)
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
                    break;
                }
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

        private:
            uint pin_;
        };
    }
}