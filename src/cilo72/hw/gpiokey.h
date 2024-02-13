/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/
 
#pragma once

#include "pico/stdlib.h"
#include "cilo72/hw/repeating_timer.h"
#include "cilo72/hw/debounce.h"
#include "cilo72/hw/gpio.h"

namespace cilo72
{
  namespace hw
  {
    /**
     * @class GpioKey
     * @brief A class for debouncing inputs
     */
    class GpioKey
    {
    public:
      /**
       * @brief Constructor for the GpioKey class
       */
      GpioKey(uint8_t pinKey, Gpio::Pull pull);

      /**
       * Check if the button is pressed.
       * @return True if the button is pressed, false otherwise.
       */
      bool isPressed() const;

      /**
       * Check if the button is pressed.
       * @return True if the button is pressed, false otherwise.
       */
      operator const bool & () const 
      { 
        return down_; 
      }

      /**
       * Check if the button is pressed.
       * @return True if the button is pressed, false otherwise.
       */
      bool pressed();

    private:
      cilo72::hw::RepeatingTimer repeatingTimer_; ///< Timer used for debouncing.
      Gpio gpio_;                                 ///< Pin for detecting button press.
      bool down_;                                 ///< Current button state.
      bool lastDown_;
      cilo72::hw::Debounce debounceSW_;           ///< Debounce object for button press.
      bool pressed_;

      void timerCallback();
    };
  }
}