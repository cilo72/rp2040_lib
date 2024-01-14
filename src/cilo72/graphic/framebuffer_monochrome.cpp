/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/graphic/framebuffer_monochrome.h"

namespace cilo72
{
  namespace graphic
  {
      FramebufferMonochrome::FramebufferMonochrome(uint8_t width, uint8_t height)
          : Framebuffer(width, height, width * height / 8)
      {
      }

      void FramebufferMonochrome::clear(const Color &color)
      {
        if (color == Color::white)
        {
          memset(buffer_, 0xFF, bufferSize_);
        }
        else
        {
          memset(buffer_, 0x00, bufferSize_);
        }
      }

      void FramebufferMonochrome::drawPixel(uint8_t x, uint8_t y, const Color &color)
      {
        if (color == Color::white)
        {
          buffer_[x + width_ * (y >> 3)] |= 0x1 << (y & 0x07);
        }
        else
        {
          buffer_[x + width_ * (y >> 3)] &= ~(0x1 << (y & 0x07));
        }
      }
  }
}
