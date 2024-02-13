/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>
#include <string.h>
#include "cilo72/fonts/font_8x5.h"
#include "color.h"

namespace cilo72
{
  namespace graphic
  {
    class Framebuffer
    {
    public:
      enum Position
      {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center,
        CenterLeft,
        CenterRight
      };
      Framebuffer(uint8_t width, uint8_t height, size_t bufferSize);
      virtual void clear(const Color &color = Color(0, 0, 0)) = 0;

      /**
       * @brief Draw a pixel on the display.
       * @param x The X coordinate.
       * @param y The Y coordinate.
       */
      virtual void drawPixel(uint8_t x, uint8_t y, const Color &color) = 0;

      /*!
       * @brief Get the framebuffer.
       * @return The framebuffer.
       */
      const uint8_t *buffer() const;

      /*!
       * @brief Get the width of the framebuffer.
       * @return The width of the framebuffer.
       */
      uint8_t width() const;

      /*!
       * @brief Get the height of the framebuffer.
       * @return The height of the framebuffer.
       */
      uint8_t height() const;

      /*!
       * @brief Get the size of the framebuffer in bytes.
       * @return The size of the framebuffer in bytes.
       * @note This is not the size of the display in pixels, but the size of the framebuffer in bytes.
       */
      size_t bufferSize() const;

      /**
       * @brief Fill the framebuffer with a value.
       * @param value The value to fill the framebuffer with.
       */
      void fill(uint8_t value);

      /*!
       * @brief Overload of the [] operator to access a pixel in the framebuffer.
       *
       * @param index The index of the pixel.
       * @return The pixel at the given index.
       * @warning An out of bounds index will result in an assert!
       */
      uint8_t &operator[](size_t index);

      /*!
       * @brief Overload of the [] operator to access a pixel in the framebuffer.
       *
       * @param index The index of the pixel.
       * @return The pixel at the given index.
       * @warning An out of bounds index will result in an assert!
       */
      const uint8_t &operator[](size_t index) const;

      /**
       * @brief Draw a filled square on the display.
       *
       * @param x The X coordinate of the top-left corner of the square.
       * @param y The Y coordinate of the top-left corner of the square.
       * @param width The width of the square.
       * @param height The height of the square.
       */
      void drawSquare(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color);

      /**
       * @brief Draw a line on the display.
       *
       * @param x1 The starting X coordinate.
       * @param y1 The starting Y coordinate.
       * @param x2 The ending X coordinate.
       * @param y2 The ending Y coordinate.
       */
      void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);

      /**
       * @brief Draw an empty square on the display.
       *
       * @param x The X coordinate of the top-left corner of the square.
       * @param y The Y coordinate of the top-left corner of the square.
       * @param width The width of the square.
       * @param height The height of the square.
       */
      void drawEmptySquare(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color);

      /**
       * @brief Draw a character on the display.
       *
       * @param x The X coordinate.
       * @param y The Y coordinate.
       * @param scale The character scaling factor.
       * @param c The character to draw.
       * @param font The font to use. Default is cilo72::fonts::Font8x5().
       */
      void drawChar(uint32_t x, uint32_t y, uint32_t scale, char c, Color color, const cilo72::fonts::Font &font);

      /**
       * @brief Draw a string on the display.
       *
       * @param x The X coordinate.
       * @param y The Y coordinate.
       * @param scale The character scaling factor.
       * @param s The string to draw.
       * @param font The font to use. Default is cilo72::fonts::Font8x5().
       */
      void drawString(uint32_t x, uint32_t y, uint32_t scale, const char *s, Color color = Color::white, const cilo72::fonts::Font &font = cilo72::fonts::Font8x5(), Position position = TopLeft);  

    protected:
      uint8_t *buffer_;
      uint32_t bufferSize_;
      uint8_t width_;
      uint8_t height_;

      static void swap(int32_t *a, int32_t *b);
    };
  }
}
