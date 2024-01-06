/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/hw/spi_device.h"

namespace cilo72
{
  namespace hw
  {
    SPIDevice::SPIDevice(SPIBus &spiBus, uint8_t pin_spi_csn)
        : SPIDevice(spiBus, pin_spi_csn, 1000000, 8, SPI_CPOL_1, SPI_CPHA_1)
    {
    }

    SPIDevice::SPIDevice(SPIBus &spiBus, uint8_t pin_spi_csn, uint baudrate, uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha)
        : spiBus_(spiBus), pin_spi_csn_(pin_spi_csn), baudrate_(baudrate), data_bits_(data_bits), cpol_(cpol), cpha_(cpha)
    {
      gpio_init(pin_spi_csn);
      gpio_put(pin_spi_csn, 1);
      gpio_set_dir(pin_spi_csn, GPIO_OUT);
    }

    void SPIDevice::xfer(const uint8_t *tx, uint8_t *rx, size_t len) const
    {
      spiBus_.config(baudrate_, data_bits_, cpol_, cpha_);
      csSelect();
      spi_write_read_blocking(spiBus_.instance(), tx, rx, len);
      csDeselect();
    }

    void SPIDevice::csSelect() const
    {
      asm volatile("nop \n nop \n nop"); // FIXME
      gpio_put(pin_spi_csn_, 0);
      asm volatile("nop \n nop \n nop"); // FIXME
    }

    void SPIDevice::csDeselect() const
    {
      asm volatile("nop \n nop \n nop"); // FIXME
      gpio_put(pin_spi_csn_, 1);
      asm volatile("nop \n nop \n nop"); // FIXME
    }

    void SPIDevice::setBaudrate(uint baudrate)
    {
      baudrate_ = baudrate;
    }

    void SPIDevice::setFormat(uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha)
    {
      data_bits_ = data_bits;
      cpol_ = cpol;
      cpha = cpha;
    }

  }
}