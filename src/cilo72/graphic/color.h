/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>

namespace cilo72
{
  namespace graphic
  {
    class Color
    {
    public:
      static const Color white; //< White color.
      static const Color black; //< Black color.
      static const Color red; //< Red color.
      static const Color green; //< Green color.
      static const Color blue; //< Blue color.
      static const Color yellow; //< Yellow color.
      static const Color magenta; //< Magenta color.
      static const Color cyan; //< Cyan color.

      /*!
       * @brief Create a new color.
       * @param r The red value.
       * @param g The green value.
       * @param b The blue value.
       */
      Color(uint8_t r, uint8_t g, uint8_t b);

      /*!
       * @brief Compare two colors.
       * @param rhs The color to compare with.
       * @return True if the colors are equal.
       */
      bool operator==(const Color &rhs) const;

      /*!
       * @brief Convert a color to RGB565.
       * @param color The color to convert.
       * @param swapBytes True if the bytes should be swapped.
       * @return The color in RGB565 format.
       */
      uint16_t toRGB565(const Color &color, bool swapBytes = false) const;

      uint8_t r() const { return r_; }
      uint8_t g() const { return g_; }
      uint8_t b() const { return b_; }

    protected:
      uint8_t r_;
      uint8_t g_;
      uint8_t b_;
    };
  }
}
