/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "color.h"

namespace cilo72
{
namespace graphic
{
        const Color Color::white    = Color(255, 255, 255);
        const Color Color::black    = Color(  0,   0,   0);
        const Color Color::red      = Color(255,   0,   0);
        const Color Color::green    = Color(  0, 255,   0);
        const Color Color::blue     = Color(  0,   0, 255);
        const Color Color::yellow   = Color(255, 255,   0);
        const Color Color::magenta  = Color(255,   0, 255);
        const Color Color::cyan     = Color(  0, 255, 255);
        
        Color::Color(uint8_t r, uint8_t g, uint8_t b) : r_(r), g_(g), b_(b)
        {
        }

        bool Color::operator==(const Color &rhs) const
        {
            return r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_;
        }

        uint16_t Color::toRGB565(const Color &color, bool swapBytes) const
        {
            uint16_t ret = (color.r_ >> 3) << 11 | (color.g_ >> 2) << 5 | (color.b_ >> 3);
            if(swapBytes)
            {
                return (ret >> 8) | (ret << 8);
            }
            return ret;
        }

}
}
