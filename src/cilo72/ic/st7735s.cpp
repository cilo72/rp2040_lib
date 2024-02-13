/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/ic/st7735s.h"

namespace cilo72
{
    namespace ic
    {
        ST7735S::ST7735S(cilo72::graphic::FramebufferRGB565 & fb, cilo72::hw::SPIDevice &spi, uint8_t pinDC, uint8_t pinRST, uint8_t pinBL)
            : fb_(fb), spi_(spi), pinDC_(pinDC, cilo72::hw::Gpio::Direction::Output, cilo72::hw::Gpio::Level::Low), pinRST_(pinRST, cilo72::hw::Gpio::Direction::Output, cilo72::hw::Gpio::Level::High), pinBL_(pinBL), scanDirection_(ScanDirection::Horizontal), swap_(false)
        {
            union 
            {
                uint16_t data;
                uint8_t tx[2];
            } __attribute__((packed));

            tx[0] = 0x55;
            tx[1] = 0xAA;

            swap_ = data == 0xAA55;
            fb.setSwapBytes(swap_);
            
            spi_.setBaudrate(10000000);
            spi_.setFormat(8, SPI_CPOL_0, SPI_CPHA_0);

            pinRST_.clear();
            sleep_ms(100);
            pinRST_.set();

            pinBL_.setFrequency(100);
            pinBL_.setDutyCycleU32(100);
            pinBL_.enable();
        }

        void ST7735S::init() const
        {
            reset();

            cmdMemoryDataAccessControl(false, true, true, true, true, false);
            cmdInterfacePixelFormat(ColorMode::RGB565);
            cmdFrameRateControlNormalMode(0x01, 0x2C, 0x2D); // Frame Rate = 95Hz
            cmdFrameRateControlIdleMode(0x01, 0x2C, 0x2D); // Frame Rate = 95Hz
            cmdFrameRateControlPartialMode(0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D); // Frame Rate = 95Hz
            cmdDisplayInversionControl(true, true, true);
            cmdPowerControl1(5, 2, 2, 2);
            cmdPowerControl2(3, 1, 1);
            cmdPowerControl3(2, 1, 0);
            cmdPowerControl4(2, 1, 0x22A);
            cmdPowerControl5(2, 1, 0x2EE);
            cmdVCOMControl1(0x0E);
            cmdGammaPlusPolarityCorrection(0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10);
            cmdGammaMinusPolarityCorrection(0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10);
            cmdSleepOut();            
            cmdDisplayOn();            
        }

        void ST7735S::clear(const cilo72::graphic::Color &Color) const
        {
            uint16_t data = Color.toRGB565(Color, swap_);
            union PixelData
            {
                uint16_t data;
                uint8_t tx[2];
            } __attribute__((packed));

            PixelData pixelData = { .data = data };

            cmdAaddressSet(0, 0, MAX_WIDTH, MAX_HEIGHT);

            cmd(CMD_RAMWR, nullptr, 0);
            pinDC_.set();
            spi_.write(pixelData.tx, sizeof(pixelData.tx), MAX_WIDTH * MAX_HEIGHT * 2);
        }

        void ST7735S::update() const
        {
            cmdAaddressSet(0, 0, fb_.width(), fb_.height());

            cmd(CMD_RAMWR, nullptr, 0);
            pinDC_.set();
            spi_.write(fb_.buffer(), fb_.bufferSize());
        }

        void ST7735S::cmdAaddressSet(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd) const
        {
            uint8_t x;
            uint8_t y;

            if (scanDirection_ == ScanDirection::Horizontal)
            {
                x = 1;
                y = 2;
            }
            else
            {
                x = 2;
                y = 1;
            }
            cmdColumnAddressSet(xStart + x, xEnd - 1 + x);
            cmdRowAddressSet(yStart + y, yEnd - 1 + y);
        }

        void ST7735S::reset() const
        {
            pinRST_.clear();
            sleep_ms(100);
            pinRST_.set();
            sleep_ms(100);
        }

        void ST7735S::cmd(CMD cmd, const uint8_t* tx, size_t len) const
        {
            uint8_t txCmd[1] = {0};
            txCmd[0] = cmd;

            pinDC_.clear();
            spi_.write(txCmd, sizeof(txCmd));
            if(tx != nullptr && len > 0)
            {
                pinDC_.set();
                spi_.write(tx, len);
            }
        }

        void ST7735S::cmdColumnAddressSet(uint16_t xStart, uint16_t xEnd) const
        {
            uint8_t tx[4] = {0};

            tx[0] = (xStart >> 8) & 0xFF;
            tx[1] = (xStart >> 0) & 0xFF;
            tx[2] = (xEnd >> 8) & 0xFF;
            tx[3] = (xEnd >> 0) & 0xFF;

            cmd(CMD_CASET, tx, sizeof(tx));
        }

        void ST7735S::cmdRowAddressSet(uint16_t yStart, uint16_t yEnd) const
        {
            uint8_t tx[4] = {0};

            tx[0] = (yStart >> 8) & 0xFF;
            tx[1] = (yStart >> 0) & 0xFF;
            tx[2] = (yEnd >> 8) & 0xFF;
            tx[3] = (yEnd >> 0) & 0xFF;

            cmd(CMD_RASET, tx, sizeof(tx));
        }

        void ST7735S::cmdDisplayOn() const
        {
            cmd(CMD_DISPON, nullptr, 0);
        }

        void ST7735S::cmdDisplayOff() const
        {
            cmd(CMD_DISPOFF, nullptr, 0);
        }

        void ST7735S::cmdSleepOut() const
        {
            cmd(CMD_SLPOUT, nullptr, 0);
            sleep_ms(120);
        }

        void ST7735S::cmdInterfacePixelFormat(ColorMode colorMode) const
        {
            uint8_t tx[1] = {0};

            tx[0] = static_cast<uint8_t>(colorMode);

            cmd(CMD_COLMOD, tx, sizeof(tx));
        }

        void ST7735S::cmdFrameRateControlNormalMode(uint8_t rtna, uint8_t fpa, uint8_t bpa) const
        {
            uint8_t tx[3] = {0};

            tx[0] = rtna & 0x0F;
            tx[1] = fpa & 0x3F;
            tx[2] = bpa & 0x3F;;

            cmd(CMD_FRMCTR1, tx, sizeof(tx));
        }

        void ST7735S::cmdFrameRateControlIdleMode(uint8_t rtnb, uint8_t fpb, uint8_t bpb) const
        {
            uint8_t tx[3] = {0};

            tx[0] = rtnb & 0x0F;
            tx[1] = fpb & 0x3F;
            tx[2] = bpb & 0x3F;;

            cmd(CMD_FRMCTR2, tx, sizeof(tx));
        }

        void ST7735S::cmdFrameRateControlPartialMode(uint8_t rtnc, uint8_t fpc, uint8_t bpc, uint8_t rtnd, uint8_t fdc, uint8_t bpd) const
        {
            uint8_t tx[6] = {0};

            tx[0] = rtnc & 0x0F;
            tx[1] = fpc & 0x3F;
            tx[2] = bpc & 0x3F;
            tx[3] = rtnd & 0x0F;
            tx[4] = fdc & 0x3F;
            tx[5] = bpd & 0x3F;

            cmd(CMD_FRMCTR3, tx, sizeof(tx));
        }

        void ST7735S::cmdDisplayInversionControl(bool nla, bool nlb, bool nlc) const
        {
            uint8_t tx[1] = {0};

            tx[0] = (nla << 2) | (nlb << 1) | (nlc << 0);

            cmd(CMD_INVCTR, tx, sizeof(tx));
        }

        void ST7735S::cmdPowerControl1(uint8_t avdd, uint8_t vrhp, uint8_t vrhn, uint8_t mode) const
        {
            uint8_t tx[3] = {0};

            tx[0] = ((avdd & 0x07) << 5) | (vrhp & 0x1F);
            tx[1] = vrhn & 0x1F;
            tx[2] = ((mode & 0x03) << 6) | 0x04 | ((vrhn >> 5) << 1) | ((vrhp >> 5) << 0);

            cmd(CMD_PWCTR1, tx, sizeof(tx));
        }

        void ST7735S::cmdPowerControl2(uint8_t vgh25, uint8_t vglsel, uint8_t vghbt) const
        {
            uint8_t tx[1] = {0};

            tx[0] = ((vgh25 & 0x03) << 6) | ((vglsel & 0x03) << 2) | (vghbt & 0x03);

            cmd(CMD_PWCTR2, tx, sizeof(tx));
        }

        void ST7735S::cmdPowerControl3(uint8_t apa, uint8_t sapa, uint16_t dca) const
        {
            uint8_t tx[2] = {0};

            tx[0] = (((dca > 8) & 0x03) << 6) | ((sapa & 0x03) << 3) | (apa & 0x07);
            tx[1] = dca & 0xFF;

            cmd(CMD_PWCTR3, tx, sizeof(tx));
        }

       void ST7735S::cmdPowerControl4(uint8_t apb, uint8_t sapb, uint16_t dcb) const
        {
            uint8_t tx[2] = {0};

            tx[0] = (((dcb >> 8) & 0x03) << 6) | ((sapb & 0x07) << 3) | (apb & 0x07);
            tx[1] = dcb & 0xFF;

            cmd(CMD_PWCTR4, tx, sizeof(tx));
        }

        void ST7735S::cmdPowerControl5(uint8_t apc, uint8_t sapc, uint16_t dcc) const
        {
            uint8_t tx[2] = {0};

            tx[0] = (((dcc >> 8) & 0x03) << 6) | ((sapc & 0x07) << 3) | (apc & 0x07);
            tx[1] = dcc & 0xFF;

            cmd(CMD_PWCTR5, tx, sizeof(tx));
        }

        void ST7735S::cmdVCOMControl1(uint8_t vcoms) const
        {
            uint8_t tx[1] = {0};

            tx[0] = vcoms & 0x3F;

            cmd(CMD_VMCTR1, tx, sizeof(tx));
        }

        void ST7735S::cmdGammaPlusPolarityCorrection(uint8_t vrf0p, uint8_t vos0p, uint8_t pk0p, uint8_t pk1p, uint8_t pk2p, uint8_t pk3p, uint8_t pk4p, uint8_t pk5p, uint8_t pk6p, uint8_t pk7p, uint8_t pk8p, uint8_t pk9p, uint8_t selv0p, uint8_t selv1p, uint8_t selv62p, uint8_t selv63p) const
        {
            uint8_t tx[16] = {0};

            tx[0] = vrf0p & 0x3F;
            tx[1] = vos0p & 0x3F;
            tx[2] = pk0p & 0x3F;
            tx[3] = pk1p & 0x3F;
            tx[4] = pk2p & 0x3F;
            tx[5] = pk3p & 0x3F;
            tx[6] = pk4p & 0x3F;
            tx[7] = pk5p & 0x3F;
            tx[8] = pk6p & 0x3F;
            tx[9] = pk7p & 0x3F;
            tx[10] = pk8p & 0x3F;
            tx[11] = pk9p & 0x3F;
            tx[12] = selv0p & 0x3F;
            tx[13] = selv1p & 0x3F;
            tx[14] = selv62p & 0x3F;
            tx[15] = selv63p & 0x3F;

            cmd(CMD_GMCTRP1, tx, sizeof(tx));
        }

        void ST7735S::cmdGammaMinusPolarityCorrection(uint8_t vrf0n, uint8_t vos0n, uint8_t pk0n, uint8_t pk1n, uint8_t pk2n, uint8_t pk3n, uint8_t pk4n, uint8_t pk5n, uint8_t pk6n, uint8_t pk7n, uint8_t pk8n, uint8_t pk9n, uint8_t selv0n, uint8_t selv1n, uint8_t selv62n, uint8_t selv63n) const
        {
            uint8_t tx[16] = {0};

            tx[0] = vrf0n & 0x3F;
            tx[1] = vos0n & 0x3F;
            tx[2] = pk0n & 0x3F;
            tx[3] = pk1n & 0x3F;
            tx[4] = pk2n & 0x3F;
            tx[5] = pk3n & 0x3F;
            tx[6] = pk4n & 0x3F;
            tx[7] = pk5n & 0x3F;
            tx[8] = pk6n & 0x3F;
            tx[9] = pk7n & 0x3F;
            tx[10] = pk8n & 0x3F;
            tx[11] = pk9n & 0x3F;
            tx[12] = selv0n & 0x3F;
            tx[13] = selv1n & 0x3F;
            tx[14] = selv62n & 0x3F;
            tx[15] = selv63n & 0x3F;

            cmd(CMD_GMCTRN1, tx, sizeof(tx));
        }

        void ST7735S::cmdMemoryDataAccessControl(bool my, bool mx, bool mv, bool ml, bool rgb, bool mh) const
        {
            uint8_t tx[1] = {0};

            tx[0] = (my << 7) | (mx << 6) | (mv << 5) | (ml << 4) | (rgb << 3) | (mh << 2);

            cmd(CMD_MADCTL, tx, sizeof(tx));
        }

        void ST7735S::setBacklight(uint32_t level) const
        {
            pinBL_.setDutyCycleU32(level);
        }
    }
}