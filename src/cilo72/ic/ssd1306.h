/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0

  This implementation is heavily inspired by https://github.com/daschr/pico-ssd1306.
*/

#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "cilo72/hw/i2c_bus.h"
#include "cilo72/fonts/font_8x5.h"
#include "cilo72/graphic/framebuffer_monochrome.h"

namespace cilo72
{
  namespace ic
  {
    /**
     * @brief Class for controlling an SSD1306 OLED display via I2C.
     * \see  <a href="https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf">Datasheet</a>
     */
    class SSD1306
    {
    public:
      /**
       * @brief Construct a new SSD1306 object.
       *
       * @param i2cBus The I2C bus to use for communication with the display.
       * @param useSA0 If true, the display's SA0 pin is tied to GND.
       */
      SSD1306(cilo72::graphic::FramebufferMonochrome & fb, const cilo72::hw::I2CBus &i2cBus, bool useSA0 = true);

      /**
       * @brief Update the display with the contents of the buffer.
       *
       */
      void update();

      /**
       * @brief Turns off the display.
       */
      void powerOff();

      /**
       * @brief Turns on the display.
       */
      void powerOn();

      /**
       * @brief Sets the contrast of the display.
       * @param val The contrast value (0-255).
       */
      void contrast(uint8_t val);

      /**
       * @brief Inverts the display.
       * @param inv Whether to invert the display (true) or not (false).
       */
      void invert(uint8_t inv);

       /** 
        * @brief Returns the framebuffer of the display.
        * @return The framebuffer of the display.
        */
      cilo72::graphic::FramebufferMonochrome & framebuffer();
      
    private:
      const cilo72::hw::I2CBus &i2cBus_;
      cilo72::graphic::FramebufferMonochrome & fb_;
      bool external_vcc_;
      uint8_t address_;
      uint8_t pages_;

      /**
       * @brief Initializes the display.
       * @return True if initialization was successful, false otherwise.
       */
      bool init();

      /**
       * @brief Writes a value to the display.
       * @param val The value to write.
       * @return True if the write was successful, false otherwise.
       */
      bool write(uint8_t val);
    };
  }
}
