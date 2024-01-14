/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "cilo72/hw/spi_device.h"
#include "cilo72/hw/gpio.h"
#include "cilo72/graphic/framebuffer_rgb565.h"

namespace cilo72
{
    namespace ic
    {
        /*!
         * @brief Display driver ST7735S. 162*132 Pixel, SPI, RGB565
         *   @see https://www.crystalfontz.com/controllers/Sitronix/ST7735S/320/
         */
        class ST7735S
        {
        public:
            static constexpr uint32_t MAX_WIDTH = 162; //<Maximum width 162 Pixel
            static constexpr uint32_t MAX_HEIGHT = 132; //<Maximum height 132 Pixel

            /*!
             * @brief Constructor
             * @param fb Framebuffer
             * @param spi SPI device
             * @param pinDC Pin DC
             * @param pinRST Pin RST
             * @param pinBL Pin BL
             */
            ST7735S(cilo72::graphic::FramebufferRGB565 &fb, cilo72::hw::SPIDevice &spi, uint8_t pinDC, uint8_t pinRST, uint8_t pinBL);

            /*!
             * @brief Reset display
             */
            void reset() const;
            /*!
            * @brief Initialize display
            */
            void init() const;
            
            /*!
             * @brief Clear display and fill with given color
             * @param color Color
             */
            void clear(const cilo72::graphic::Color &color) const;

            /*!
             * @brief Update display
             */
            void update() const;

            /*!
             * @brief Get framebuffer
             * @return Framebuffer
             */
            cilo72::graphic::FramebufferRGB565 & framebuffer() const { return fb_;}

        protected:
            enum ScanDirection
            {
                Horizontal,
                Vertical,
            };

            enum CMD
            {
                CMD_SLPOUT = 0x11,
                CMD_DISPOFF = 0x28,
                CMD_DISPON = 0x29,
                CMD_CASET = 0x2A,
                CMD_RASET = 0x2B,
                CMD_RAMWR = 0x2C,
                CMD_MADCTL = 0x36,
                CMD_COLMOD = 0x3A,
                CMD_FRMCTR1 = 0xB1,
                CMD_FRMCTR2 = 0xB2,
                CMD_FRMCTR3 = 0xB3,
                CMD_INVCTR = 0xB4,
                CMD_PWCTR1 = 0xC0,
                CMD_PWCTR2 = 0xC1,
                CMD_PWCTR3 = 0xC2,
                CMD_PWCTR4 = 0xC3,
                CMD_PWCTR5 = 0xC4,
                CMD_VMCTR1 = 0xC5,
                CMD_GMCTRP1 = 0xE0,
                CMD_GMCTRN1 = 0xE1,
            };

            enum class ColorMode
            {
                RGB444 = 0x03,
                RGB565 = 0x05,
                RGB666 = 0x06,
            };

            cilo72::graphic::FramebufferRGB565 &fb_;
            cilo72::hw::SPIDevice &spi_;
            cilo72::hw::Gpio pinDC_;
            cilo72::hw::Gpio pinRST_;
            cilo72::hw::Gpio pinBL_;
            ScanDirection scanDirection_;
            bool swap_;

            void cmd(CMD cmd, const uint8_t *tx, size_t len) const;

            void cmdMemoryDataAccessControl(bool my, bool mx, bool mv, bool ml, bool rgb, bool mh) const;
            void cmdColumnAddressSet(uint16_t xStart, uint16_t xEnd) const;
            void cmdRowAddressSet(uint16_t xStart, uint16_t xEnd) const;
            void cmdDisplayOn() const;
            void cmdDisplayOff() const;
            void cmdSleepOut() const;
            void cmdInterfacePixelFormat(ColorMode colorMode) const;
            void cmdFrameRateControlNormalMode(uint8_t rtna, uint8_t fpa, uint8_t bpa) const;
            void cmdFrameRateControlIdleMode(uint8_t rtnb, uint8_t fpb, uint8_t bpb) const;
            void cmdFrameRateControlPartialMode(uint8_t rtnc, uint8_t fpc, uint8_t bpc, uint8_t rtnd, uint8_t fdc, uint8_t bpd) const;
            void cmdDisplayInversionControl(bool nla, bool nlb, bool nlc) const;
            void cmdPowerControl1(uint8_t avdd, uint8_t vrhp, uint8_t vrhn, uint8_t mode) const;
            void cmdPowerControl2(uint8_t vgh25, uint8_t vglsel, uint8_t vghbt) const;
            void cmdPowerControl3(uint8_t apa, uint8_t sapa, uint16_t dca) const;
            void cmdPowerControl4(uint8_t apb, uint8_t sapb, uint16_t dcb) const;
            void cmdPowerControl5(uint8_t apc, uint8_t sapc, uint16_t dcc) const;
            void cmdVCOMControl1(uint8_t vcoms) const;
            void cmdGammaPlusPolarityCorrection(uint8_t vrf0p, uint8_t vos0p, uint8_t pk0p, uint8_t pk1p, uint8_t pk2p, uint8_t pk3p, uint8_t pk4p, uint8_t pk5p, uint8_t pk6p, uint8_t pk7p, uint8_t pk8p, uint8_t pk9p, uint8_t selv0p, uint8_t selv1p, uint8_t selv62p, uint8_t selv63p) const;
            void cmdGammaMinusPolarityCorrection(uint8_t vrf0n, uint8_t vos0n, uint8_t pk0n, uint8_t pk1n, uint8_t pk2n, uint8_t pk3n, uint8_t pk4n, uint8_t pk5n, uint8_t pk6n, uint8_t pk7n, uint8_t pk8n, uint8_t pk9n, uint8_t selv0n, uint8_t selv1n, uint8_t selv62n, uint8_t selv63n) const;
            void cmdAaddressSet(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd) const;
        };
    }
}
