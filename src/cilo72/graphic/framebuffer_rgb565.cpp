/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/graphic/framebuffer_rgb565.h"

namespace cilo72
{
  namespace graphic
  {
      FramebufferRGB565::FramebufferRGB565(uint8_t width, uint8_t height)
          : Framebuffer(width, height, width * height * 2)
          , swapBytes_(false)
      {
      }

      void FramebufferRGB565::clear(const Color &color)
      {
        uint16_t color565 = color.toRGB565(color, swapBytes_);
        uint16_t *buffer = (uint16_t *)buffer_;
        for(uint32_t i = 0; i < height_ * width_; i++)
        {
          buffer[i] = color565;
        }
      }

      void FramebufferRGB565::drawPixel(uint8_t x, uint8_t y, const Color &color)
      {
        uint16_t *buffer = (uint16_t *)buffer_;
        buffer[y * width_ + x] = color.toRGB565(color, swapBytes_);
      }
  }
}
