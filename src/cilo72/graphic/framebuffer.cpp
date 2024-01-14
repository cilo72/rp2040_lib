/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "framebuffer.h"

namespace cilo72
{
  namespace graphic
  {
    Framebuffer::Framebuffer(uint8_t width, uint8_t height, size_t bufferSize)
        : buffer_(nullptr), bufferSize_(bufferSize), width_(width), height_(height)
    {
      buffer_ = new uint8_t[bufferSize_];
    }

    const uint8_t *Framebuffer::buffer() const
    {
      return buffer_;
    }

    uint8_t Framebuffer::width() const
    {
      return width_;
    }

    uint8_t Framebuffer::height() const
    {
      return height_;
    }

    size_t Framebuffer::bufferSize() const
    {
      return bufferSize_;
    }

    void Framebuffer::fill(uint8_t value)
    {
      memset(buffer_, value, bufferSize_);
    }

    void Framebuffer::swap(int32_t *a, int32_t *b)
    {
      int32_t *t = a;
      *a = *b;
      *b = *t;
    }

    /*!
     * Overload [] operator to access data directly.
     * \param index Index of the data to access
     * \return Data at the given index
     */
    uint8_t &Framebuffer::operator[](size_t index)
    {
      if (index >= bufferSize_)
      {
        assert(0);
      }
      return buffer_[index];
    }

    /*!
     * Overload [] operator to access data directly (const version).
     * \param index Index of the data to access
     * \return Data at the given index
     */
    const uint8_t &Framebuffer::operator[](size_t index) const
    {
      if (index >= bufferSize_)
      {
        assert(0);
      }
      return buffer_[index];
    }

    void Framebuffer::drawSquare(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color)
    {
      for (uint32_t i = 0; i < width; ++i)
      {
        for (uint32_t j = 0; j < height; ++j)
        {
          drawPixel(x + i, y + j, color);
        }
      }
    }

    void Framebuffer::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
    {
      if (x1 > x2)
      {
        swap(&x1, &x2);
        swap(&y1, &y2);
      }

      if (x1 == x2)
      {
        if (y1 > y2)
        {
          swap(&y1, &y2);
        }

        for (int32_t i = y1; i <= y2; ++i)
        {
          drawPixel(x1, i, color);
        }
        return;
      }

      float m = (float)(y2 - y1) / (float)(x2 - x1);

      for (int32_t i = x1; i <= x2; ++i)
      {
        float y = m * (float)(i - x1) + (float)y1;
        drawPixel(i, (uint32_t)y, color);
      }
    }

    void Framebuffer::drawEmptySquare(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color)
    {
      drawLine(x, y, x + width, y, color);
      drawLine(x, y + height, x + width, y + height, color);
      drawLine(x, y, x, y + height, color);
      drawLine(x + width, y, x + width, y + height, color);
    }

    void Framebuffer::drawChar(uint32_t x, uint32_t y, uint32_t scale, char c, Color color, const cilo72::fonts::Font &font)
    {
      if (c < font.firstAscciiChar() || c > font.lastAscciiChar())
      {
        return;
      }

      uint32_t parts_per_line = (font.height() >> 3) + ((font.height() & 7) > 0);
      for (uint8_t w = 0; w < font.width(); ++w)
      { // width
        uint32_t pp = (c - font.firstAscciiChar()) * font.width() * parts_per_line + w * parts_per_line;
        for (uint32_t lp = 0; lp < parts_per_line; ++lp)
        {
          uint8_t line = font.data()[pp];

          for (int8_t j = 0; j < 8; ++j, line >>= 1)
          {
            if (line & 1)
            {
              drawSquare(x + w * scale, y + ((lp << 3) + j) * scale, scale, scale, color);
            }
          }
          ++pp;
        }
      }
    }

    void Framebuffer::drawString(uint32_t x, uint32_t y, uint32_t scale, const char *s, Color color, const cilo72::fonts::Font &font)
    {
      for (int32_t x_n = x; *s; x_n += (font.width() + font.spacingPerChar()) * scale)
      {
        drawChar(x_n, y, scale, *(s++), color, font);
      }
    }

  }
}