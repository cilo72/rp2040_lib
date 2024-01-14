/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/ic/ssd1306.h"
#include "cilo72/fonts/font.h"
#include <string.h>

namespace cilo72
{
    namespace
    {
        typedef enum
        {
            SET_CONTRAST = 0x81,
            SET_ENTIRE_ON = 0xA4,
            SET_NORM_INV = 0xA6,
            SET_DISP = 0xAE,
            SET_MEM_ADDR = 0x20,
            SET_COL_ADDR = 0x21,
            SET_PAGE_ADDR = 0x22,
            SET_DISP_START_LINE = 0x40,
            SET_SEG_REMAP = 0xA0,
            SET_MUX_RATIO = 0xA8,
            SET_COM_OUT_DIR = 0xC0,
            SET_DISP_OFFSET = 0xD3,
            SET_COM_PIN_CFG = 0xDA,
            SET_DISP_CLK_DIV = 0xD5,
            SET_PRECHARGE = 0xD9,
            SET_VCOM_DESEL = 0xDB,
            SET_CHARGE_PUMP = 0x8D
        } ssd1306_command_t;

        constexpr uint8_t address0 = 0x3C;
        constexpr uint8_t address1 = 0x3D;
    }

    namespace ic
    {
        SSD1306::SSD1306(cilo72::graphic::FramebufferMonochrome & fb, const cilo72::hw::I2CBus &i2cBus, bool useSA0)
            : i2cBus_(i2cBus)
            , fb_(fb)
            , external_vcc_(false)
            , address_(useSA0 ? address0 : address1)
            , pages_(fb.height() / 8)
        {
            init();            
            update();
        }

        bool SSD1306::init()
        {
            uint8_t cmds[] = {
                SET_DISP,
                // timing and driving scheme
                SET_DISP_CLK_DIV,
                0x80,
                SET_MUX_RATIO,
                uint8_t(fb_.height() - 1),
                SET_DISP_OFFSET,
                0x00,
                // resolution and layout
                SET_DISP_START_LINE,
                // charge pump
                SET_CHARGE_PUMP,
                (uint8_t)(external_vcc_ ? 0x10 : 0x14),
                SET_SEG_REMAP | 0x01,   // column addr 127 mapped to SEG0
                SET_COM_OUT_DIR | 0x08, // scan from COM[N] to COM0
                SET_COM_PIN_CFG,
                (uint8_t)(fb_.width() > 2 * fb_.height() ? 0x02 : 0x12),
                // display
                SET_CONTRAST,
                0xff,
                SET_PRECHARGE,
                (uint8_t)(external_vcc_ ? 0x22 : 0xF1),
                SET_VCOM_DESEL,
                0x30,          // or 0x40?
                SET_ENTIRE_ON, // output follows RAM contents
                SET_NORM_INV,  // not inverted
                SET_DISP | 0x01,
                // address setting
                SET_MEM_ADDR,
                0x00, // horizontal
            };

            for (size_t i = 0; i < sizeof(cmds); ++i)
            {
                write(cmds[i]);
            }
            return true;
        }

        void SSD1306::update()
        {
            uint8_t payload[] = {SET_COL_ADDR, 0, (uint8_t)(fb_.width() - 1), SET_PAGE_ADDR, 0, (uint8_t)(pages_ - 1)};
            if (fb_.width() == 64)
            {
                payload[1] += 32;
                payload[2] += 32;
            }

            for (size_t i = 0; i < sizeof(payload); ++i)
            {
                write(payload[i]);
            }

            i2cBus_.writeBlocking(address_, [&](size_t index, uint8_t & byte) -> bool
            {
                if(index == 0)
                {
                    byte = 0x40;
                }
                else
                {
                    byte = fb_.buffer()[index - 1];
                }


                return index < fb_.bufferSize()+1;
            });

        }

        bool SSD1306::write(uint8_t val)
        {
            uint8_t d[2] = {0x00, val};
            return i2cBus_.writeBlocking(address_, d, 2);
        }

        void SSD1306::powerOff()
        {
            write(SET_DISP | 0x00);
        }

        void SSD1306::powerOn()
        {
            write(SET_DISP | 0x01);
        }

        void SSD1306::contrast(uint8_t val)
        {
            write(SET_CONTRAST);
            write(val);
        }

        void SSD1306::invert(uint8_t inv)
        {
            write(SET_NORM_INV | (inv & 1));
        }

        cilo72::graphic::FramebufferMonochrome & SSD1306::framebuffer()
        {
            return fb_;
        }
    }
}
