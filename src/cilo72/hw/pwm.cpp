/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/hw/pwm.h"
#include <vector>
#include "hardware/clocks.h"

namespace cilo72
{
    namespace hw
    {
        namespace
        {

            struct SliceInstance
            {
                uint slice;
                bool used;
            };

            std::vector<SliceInstance> sliceInstances_ = {{0, false},
                                                          {1, false},
                                                          {2, false},
                                                          {3, false},
                                                          {4, false},
                                                          {5, false},
                                                          {6, false},
                                                          {7, false}};

        }

        Pwm::Pwm(uint pin)
            : Pwm(pin, PIN_NOT_USED)
        {
        }

        Pwm::Pwm(uint pinA, uint pinB)
            : slice_(0), channelA_(PIN_NOT_USED), channelB_(PIN_NOT_USED), pinA_(PIN_NOT_USED), pinB_(PIN_NOT_USED), countsPerPeriod_(0)
        {
            if (pinA != PIN_NOT_USED and pinB != PIN_NOT_USED)
            {
                uint sliceA = pwm_gpio_to_slice_num(pinA);
                uint sliceB = pwm_gpio_to_slice_num(pinB);

                if (sliceA == sliceB)
                {
                    uint channelA = pwm_gpio_to_channel(pinA);
                    uint channelB = pwm_gpio_to_channel(pinB);

                    if (channelA != channelB)
                    {
                        for (auto &sliceInstance : sliceInstances_)
                        {
                            if (sliceInstance.slice == sliceA)
                            {
                                if (sliceInstance.used == false)
                                {
                                    sliceInstance.used = true;

                                    slice_ = sliceA;
                                    pinA_ = pinA;
                                    pinB_ = pinB;
                                    channelA_ = channelA;
                                    channelB_ = channelB;

                                    gpio_set_function(pinA, GPIO_FUNC_PWM);
                                    gpio_set_function(pinB, GPIO_FUNC_PWM);
                                    pwm_set_wrap(slice_, 0xFFFF);
                                    pwm_set_chan_level(slice_, channelA_, 0);
                                    pwm_set_chan_level(slice_, channelB_, 0);
                                    setFrequency(1000);
                                    setDutyCycleU32(50);
                                    break;
                                }
                                else
                                {
                                    assert(0);
                                }
                            }
                        }
                    }
                    else
                    {
                        assert(0);
                    }
                }
                else
                {
                    assert(0);
                }
            }
            else if (pinA != PIN_NOT_USED and pinB == PIN_NOT_USED)
            {
                uint slice = pwm_gpio_to_slice_num(pinA);
                for (auto &sliceInstance : sliceInstances_)
                {
                    if (sliceInstance.slice == slice)
                    {
                        if (sliceInstance.used == false)
                        {
                            sliceInstance.used = true;

                            slice_ = slice;
                            pinA_ = pinA;
                            channelA_ = pwm_gpio_to_channel(pinA);

                            gpio_set_function(pinA, GPIO_FUNC_PWM);
                            pwm_set_wrap(slice, 0xFFFF);
                            pwm_set_chan_level(slice, channelA_, 0);
                            setFrequency(1000);
                            setDutyCycleU32(50);
                            break;
                        }
                        else
                        {
                            assert(0);
                        }
                    }
                }
            }
            else if (pinA == PIN_NOT_USED and pinB != PIN_NOT_USED)
            {
                // not yet implemented
                assert(0);
            }
            else
            {
                assert(0);
            }
        }

        void Pwm::enable() const
        {
            pwm_set_enabled(slice_, true);
        }

        bool Pwm::setFrequency(uint32_t hz)
        {
            uint32_t sysclock = clock_get_hz(clk_sys);
            static constexpr uint32_t MAX_COUNT = 0xFFFF;
            static constexpr uint32_t MAX_DIV = 256;

            uint32_t f = sysclock / MAX_COUNT;
            uint32_t div_rem = f % hz;
            uint32_t div = f / hz;

            if (div_rem > 0)
            {
                div++;
            }

            uint32_t countsPerSecond = sysclock / div;
            uint32_t countsPerPeriod = countsPerSecond / hz;

            if (countsPerPeriod > MAX_COUNT)
            {
                assert(0);
                return false;
            }

            if(div > MAX_DIV)
            {
                assert(0);
                return false;
            }

            countsPerPeriod_ = countsPerPeriod;

            pwm_set_clkdiv(slice_, div);
            pwm_set_wrap(slice_, countsPerPeriod_);
            return true;
        }

        void Pwm::setDutyCycleInCounts(uint16_t level, Operation operation) const
        {
            if (pinA_ != PIN_NOT_USED and pinB_ != PIN_NOT_USED and operation == Operation::ALL)
            {
                pwm_set_both_levels(slice_, static_cast<uint16_t>(level), static_cast<uint16_t>(level));
            }
            else if (pinA_ != PIN_NOT_USED)
            {
                if (operation == Operation::ALL or operation == Operation::A)
                {
                    pwm_set_chan_level(slice_, channelA_, static_cast<uint16_t>(level));
                }
            }
            else if (pinB_ != PIN_NOT_USED)
            {
                if (operation == Operation::ALL or operation == Operation::B)
                {
                    pwm_set_chan_level(slice_, channelB_, static_cast<uint16_t>(level));
                }
            }
        }

        bool Pwm::setDutyCycleU32(uint32_t dutyCycle, Operation operation) const
        {
            if (dutyCycle > 100)
            {
                return false;
            }

            uint32_t level = countsPerPeriod_ * dutyCycle / 100;
            setDutyCycleInCounts(level, operation);
            return true;
        }

        bool Pwm::setDutyCycleDouble(double dutyCycle, Operation operation) const
        {
            if (dutyCycle > 100.0 or dutyCycle < 0.0)
            {
                return false;
            }

            uint32_t level = static_cast<uint32_t>(static_cast<double>(countsPerPeriod_) * dutyCycle / 100.0);
            setDutyCycleInCounts(level, operation);
            return true;
        }

        void Pwm::invert(bool invert) const
        {
            pwm_set_output_polarity(slice_, invert, invert);
        }
    }
}
