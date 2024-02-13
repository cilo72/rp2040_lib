/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

namespace cilo72
{
    namespace hw
    {
        class Pwm
        {
        public:
        enum class Operation
        {
            A,
            B,
            ALL,
        };
            static constexpr uint PIN_NOT_USED = 0xFFFFFFFF;
            Pwm(uint pin);
            Pwm(uint pinA, uint pinB);
            bool setFrequency(uint32_t hz);
            bool setDutyCycleU32(uint32_t dutyCycle, Operation operation = Operation::ALL) const;
            bool setDutyCycleDouble(double dutyCycle, Operation operation = Operation::ALL) const;
            void setDutyCycleInCounts(uint16_t level, Operation operation) const;
            void enable() const;
            void invert(bool invert) const;
        protected:
            uint slice_;
            uint channelA_;
            uint channelB_;
            uint pinA_;
            uint pinB_;
            uint32_t countsPerPeriod_;
        };
    }
}
