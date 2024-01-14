/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>
#include <string.h>
#include "framebuffer.h"

namespace cilo72
{
  namespace graphic
  {
    /*!
     * @brief A framebuffer for monochrome displays.
     * @note All non-white colors are treated as black.
     */
    class FramebufferMonochrome : public Framebuffer
    {
    public:
      /*!
       * @brief Create a new framebuffer.
       * @param width The width of the framebuffer.
       * @param height The height of the framebuffer.
       */
      FramebufferMonochrome(uint8_t width, uint8_t height);

      /*!
       * @brief Clear the framebuffer.
       * @param color The color to fill the framebuffer with.
       */
      void clear(const Color &color = Color(0, 0, 0)) override;

      /*!
       * @brief Draw a pixel on the display.
       * @param x The X coordinate.
       * @param y The Y coordinate.
       */
      void drawPixel(uint8_t x, uint8_t y, const Color &color) override;
    };
  }
}
