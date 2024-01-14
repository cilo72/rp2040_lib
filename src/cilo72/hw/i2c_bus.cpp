/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/hw/i2c_bus.h"
#include "pico/stdlib.h"
#include "vector"

namespace cilo72
{
    namespace hw
    {
        namespace
        {
          struct I2CInstance
          {
            i2c_inst_t * instance;
            bool used;
          };
          std::vector<I2CInstance> i2cInstances_ = { {i2c0, false} , {i2c1, false}};
        }

        I2CBus::I2CBus(uint pin_i2c_sda, uint pin_i2c_scl)
        : i2cInstance_(nullptr)
        {
            int instance = -1;
            if(pin_i2c_sda == 0 and pin_i2c_scl == 2)
            {
                instance = 0;
            }
            else if(pin_i2c_sda == 2 and pin_i2c_scl == 3)
            {
                instance = 1;
            }
            else if(pin_i2c_sda == 4 and pin_i2c_scl == 5)
            {
                instance = 0;
            }
            else if(pin_i2c_sda == 6 and pin_i2c_scl == 7)
            {
                instance = 1;
            }
            else if(pin_i2c_sda == 8 and pin_i2c_scl == 9)
            {
                instance = 0;
            }
            else if(pin_i2c_sda == 10 and pin_i2c_scl == 11)
            {
                instance = 1;
            }
            else if(pin_i2c_sda == 12 and pin_i2c_scl == 13)
            {
                instance = 0;
            }
            else if(pin_i2c_sda == 14 and pin_i2c_scl == 15)
            {
                instance = 1;
            }
            else if(pin_i2c_sda == 16 and pin_i2c_scl == 17)
            {
                instance = 0;
            }
            else if(pin_i2c_sda == 18 and pin_i2c_scl == 19)
            {
                instance = 1;
            }
            else if(pin_i2c_sda == 20 and pin_i2c_scl == 21)
            {
                instance = 0;
            }
            else if(pin_i2c_sda == 26 and pin_i2c_scl == 27)
            {
                instance = 1;
            }
            else
            {
                assert(0);
            }

            assert(instance >= 0);

            if(i2cInstances_[instance].used == false)
            {
                i2cInstances_[instance].used = true;
                i2cInstance_ = i2cInstances_[instance].instance;
            }

            assert(i2cInstance_ != nullptr);

            i2c_init(i2cInstance_, 400 * 1000);
            gpio_set_function(pin_i2c_sda, GPIO_FUNC_I2C);
            gpio_set_function(pin_i2c_scl, GPIO_FUNC_I2C);
            gpio_pull_up(pin_i2c_sda);
            gpio_pull_up(pin_i2c_scl);
        }

        bool I2CBus::writeBlocking(uint8_t addr, std::function<bool(size_t index, uint8_t & byte)> data) const
        {
            invalid_params_if(I2C, addr >= 0x80); // 7-bit addresses
            // Synopsys hw accepts start/stop flags alongside data items in the same
            // FIFO word, so no 0 byte transfers.
            //invalid_params_if(I2C, len == 0);
            //invalid_params_if(I2C, ((int)len) < 0);

            i2cInstance_->hw->enable = 0;
            i2cInstance_->hw->tar = addr;
            i2cInstance_->hw->enable = 1;

            bool abort = false;
            bool timeout = false;
            bool nostop = false;

            uint32_t abort_reason = 0;
            int byte_ctr;

            int index = 0;
            uint8_t byte;
            bool more;
            do
            {
                more = data(index, byte);

                bool first = index == 0;
                bool last = more == false;

                i2cInstance_->hw->data_cmd = bool_to_bit(first && i2cInstance_->restart_on_next) << I2C_IC_DATA_CMD_RESTART_LSB | bool_to_bit(last && !nostop) << I2C_IC_DATA_CMD_STOP_LSB | byte;

                // Wait until the transmission of the address/data from the internal
                // shift register has completed. For this to function correctly, the
                // TX_EMPTY_CTRL flag in IC_CON must be set. The TX_EMPTY_CTRL flag
                // was set in i2c_init.
                do
                {
                    tight_loop_contents();
                } while (!timeout && !(i2cInstance_->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS));

                // If there was a timeout, don't attempt to do anything else.
                if (!timeout)
                {
                    abort_reason = i2cInstance_->hw->tx_abrt_source;
                    if (abort_reason)
                    {
                        // Note clearing the abort flag also clears the reason, and
                        // this instance of flag is clear-on-read! Note also the
                        // IC_CLR_TX_ABRT register always reads as 0.
                        i2cInstance_->hw->clr_tx_abrt;
                        abort = true;
                    }

                    if (abort || (last && !nostop))
                    {
                        // If the transaction was aborted or if it completed
                        // successfully wait until the STOP condition has occured.

                        // TODO Could there be an abort while waiting for the STOP
                        // condition here? If so, additional code would be needed here
                        // to take care of the abort.
                        do
                        {
                            tight_loop_contents();
                        } while (!timeout && !(i2cInstance_->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_STOP_DET_BITS));

                        // If there was a timeout, don't attempt to do anything else.
                        if (!timeout)
                        {
                            i2cInstance_->hw->clr_stop_det;
                        }
                    }
                }

                // Note the hardware issues a STOP automatically on an abort condition.
                // Note also the hardware clears RX FIFO as well as TX on abort,
                // because we set hwparam IC_AVOID_RX_FIFO_FLUSH_ON_TX_ABRT to 0.
                if (abort)
                    break;

                index++;
            } while (more);

            int rval;

            // A lot of things could have just happened due to the ingenious and
            // creative design of I2C. Try to figure things out.
            if (abort)
            {
                if (timeout)
                    rval = PICO_ERROR_TIMEOUT;
                else if (!abort_reason || abort_reason & I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_BITS)
                {
                    // No reported errors - seems to happen if there is nothing connected to the bus.
                    // Address byte not acknowledged
                    rval = PICO_ERROR_GENERIC;
                }
                else if (abort_reason & I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_BITS)
                {
                    // Address acknowledged, some data not acknowledged
                    rval = byte_ctr;
                }
                else
                {
                    // panic("Unknown abort from I2C instance @%08x: %08x\n", (uint32_t) i2c->hw, abort_reason);
                    rval = PICO_ERROR_GENERIC;
                }
            }
            else
            {
                rval = byte_ctr;
            }

            // nostop means we are now at the end of a *message* but not the end of a *transfer*
            i2cInstance_->restart_on_next = nostop;
            return rval;
        }

        bool I2CBus::writeBlocking(uint8_t addr, const uint8_t * src, size_t len, bool nostop) const
        {
            int ret = i2c_write_blocking(i2cInstance_, addr, src, len, nostop);
            return ret == (int)len;
        }

        bool I2CBus::readBlocking(uint8_t addr, uint8_t * dst, size_t len) const
        {
            int ret = i2c_read_blocking(i2cInstance_, addr, dst, len, false);
            return ret == (int)len;
        }

        bool I2CBus::xfer(uint8_t addr, const uint8_t * src, size_t lenSrc, uint8_t * dst, size_t lenDst) const
        {
            int retW = i2c_write_blocking(i2cInstance_, addr, src, lenSrc, true);
            int retR = i2c_read_blocking(i2cInstance_, addr, dst, lenDst, false);

            return (retW == (int)lenSrc) and (retR == (int)lenDst);
        }

    }
}
