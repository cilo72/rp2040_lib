/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "cilo72/ic/mcp2515.h"
#include <string.h>
#include<type_traits>

template<typename E>
constexpr auto toUnderlaying(E e) -> typename std::underlying_type<E>::type 
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

namespace cilo72
{
    namespace ic
    {
        namespace
        {
            static constexpr uint8_t CANCTRL_REQOP  = 0xE0;
            static constexpr uint8_t CANCTRL_ABAT   = 0x10;
            static constexpr uint8_t CANCTRL_OSM    = 0x08;
            static constexpr uint8_t CANCTRL_CLKEN  = 0x04;
            static constexpr uint8_t CANCTRL_CLKPRE = 0x03;
    
            static constexpr uint8_t CAN_MAX_DLEN   = 8;
    
            static constexpr uint8_t CANSTAT_OPMOD  = 0xE0;
    
            enum INSTRUCTION : uint8_t {
                INSTRUCTION_WRITE       = 0x02,
                INSTRUCTION_READ        = 0x03,
                INSTRUCTION_BITMOD      = 0x05,
                INSTRUCTION_LOAD_TX0    = 0x40,
                INSTRUCTION_LOAD_TX1    = 0x42,
                INSTRUCTION_LOAD_TX2    = 0x44,
                INSTRUCTION_RTS_TX0     = 0x81,
                INSTRUCTION_RTS_TX1     = 0x82,
                INSTRUCTION_RTS_TX2     = 0x84,
                INSTRUCTION_RTS_ALL     = 0x87,
                INSTRUCTION_READ_RX0    = 0x90,
                INSTRUCTION_READ_RX1    = 0x94,
                INSTRUCTION_READ_STATUS = 0xA0,
                INSTRUCTION_RX_STATUS   = 0xB0,
                INSTRUCTION_RESET       = 0xC0
            };

            static constexpr uint8_t RXBnCTRL_RXM_STD     = 0x20;
            static constexpr uint8_t RXBnCTRL_RXM_EXT     = 0x40;
            static constexpr uint8_t RXBnCTRL_RXM_STDEXT  = 0x00;
            static constexpr uint8_t RXBnCTRL_RXM_MASK    = 0x60;
            static constexpr uint8_t RXBnCTRL_RTR         = 0x08;
            static constexpr uint8_t RXB0CTRL_BUKT        = 0x04;
            static constexpr uint8_t RXB0CTRL_FILHIT_MASK = 0x03;
            static constexpr uint8_t RXB1CTRL_FILHIT_MASK = 0x07;
            static constexpr uint8_t RXB0CTRL_FILHIT      = 0x00;
            static constexpr uint8_t RXB1CTRL_FILHIT      = 0x01;

            enum  TXBnCTRL : uint8_t 
            {
                TXB_ABTF   = 0x40,
                TXB_MLOA   = 0x20,
                TXB_TXERR  = 0x10,
                TXB_TXREQ  = 0x08,
                TXB_TXIE   = 0x04,
                TXB_TXP    = 0x03
            };

            struct CanBitRateConfig
            {
                uint8_t cnf1;
                uint8_t cnf2;
                uint8_t cnf3;
            };

            static constexpr uint8_t MCP_SIDH = 0;
            static constexpr uint8_t MCP_SIDL = 1;
            static constexpr uint8_t MCP_EID8 = 2;
            static constexpr uint8_t MCP_EID0 = 3;
            static constexpr uint8_t MCP_DLC = 4;
            static constexpr uint8_t MCP_DATA = 5;

            static constexpr uint8_t TXB_EXIDE_MASK = 0x08;
            static constexpr uint8_t DLC_MASK = 0x0F;
            static constexpr uint8_t RTR_MASK = 0x40;

            enum  STAT : uint8_t
            {
                STAT_RX0IF = (1 << 0),
                STAT_RX1IF = (1 << 1)
            };
            static constexpr uint8_t STAT_RXIF_MASK = STAT_RX0IF | STAT_RX1IF;

            static CanBitRateConfig constexpr BitRate_10kBPS_16MHz   = {0x31, 0xb5, 0x01};
            static CanBitRateConfig constexpr BitRate_20kBPS_16MHz   = {0x18, 0xb5, 0x01};
            static CanBitRateConfig constexpr BitRate_50kBPS_16MHz   = {0x09, 0xb5, 0x01};
            static CanBitRateConfig constexpr BitRate_100kBPS_16MHz  = {0x04, 0xb5, 0x01};
            static CanBitRateConfig constexpr BitRate_125kBPS_16MHz  = {0x03, 0xF0, 0x86};
            static CanBitRateConfig constexpr BitRate_250kBPS_16MHz  = {0x41, 0xF1, 0x85};
            static CanBitRateConfig constexpr BitRate_500kBPS_16MHz  = {0x00, 0xF0, 0x86};
            static CanBitRateConfig constexpr BitRate_800kBPS_16MHz  = {0x00, 0x9a, 0x01};
            static CanBitRateConfig constexpr BitRate_1000kBPS_16MHz = {0x00, 0xD0, 0x82};

            static CanBitRateConfig constexpr BitRate_10kBPS_8MHz    = {0x18, 0xb5, 0x01};
            static CanBitRateConfig constexpr BitRate_20kBPS_8MHz    = {0x09, 0xbf, 0x02};
            static CanBitRateConfig constexpr BitRate_50kBPS_8MHz    = {0x04, 0xb5, 0x01};
            static CanBitRateConfig constexpr BitRate_100kBPS_8MHz   = {0x01, 0xbf, 0x02};
            static CanBitRateConfig constexpr BitRate_125kBPS_8MHz   = {0x01, 0xb1, 0x05};
            static CanBitRateConfig constexpr BitRate_250kBPS_8MHz   = {0x00, 0xb1, 0x05};
            static CanBitRateConfig constexpr BitRate_500kBPS_8MHz   = {0x00, 0x90, 0x02};
            static CanBitRateConfig constexpr BitRate_800kBPS_8MHz   = {0x00, 0x80, 0x01};
            static CanBitRateConfig constexpr BitRate_1000kBPS_8MHz  = {0x00, 0x80, 0x00};

            static CanBitRateConfig constexpr BitRate_10kBPS_10MHz   = {0x18, 0xbf, 0x02};
            static CanBitRateConfig constexpr BitRate_20kBPS_10MHz   = {0x18, 0x9a, 0x01};
            static CanBitRateConfig constexpr BitRate_50kBPS_10MHz   = {0x04, 0xbf, 0x02};
            static CanBitRateConfig constexpr BitRate_100kBPS_10MHz  = {0x04, 0x9a, 0x01};
            static CanBitRateConfig constexpr BitRate_125kBPS_10MHz  = {0x01, 0xB6, 0x04};
            static CanBitRateConfig constexpr BitRate_250kBPS_10MHz  = {0x00, 0xB6, 0x04};
            static CanBitRateConfig constexpr BitRate_500kBPS_10MHz  = {0x00, 0x9A, 0x01};
            // static CanBitRateConfig constexpr BitRate_800kBPS_10MHz  = {0x00, 0x00, 0x00}; // 10 mhz is to slow for this speed - not possible
            static CanBitRateConfig constexpr BitRate_1000kBPS_10MHz = {0x00, 0x80, 0x01};

            static CanBitRateConfig constexpr BitRate_10kBPS_12MHz   = {0x27, 0xad, 0x01};
            static CanBitRateConfig constexpr BitRate_20kBPS_12MHz   = {0x13, 0xad, 0x01};
            static CanBitRateConfig constexpr BitRate_50kBPS_12MHz   = {0x07, 0xad, 0x01};
            static CanBitRateConfig constexpr BitRate_100kBPS_12MHz  = {0x03, 0xad, 0x01};
            static CanBitRateConfig constexpr BitRate_125kBPS_12MHz  = {0x02, 0xb5, 0x01};
            static CanBitRateConfig constexpr BitRate_250kBPS_12MHz  = {0x01, 0xa3, 0x01};
            static CanBitRateConfig constexpr BitRate_500kBPS_12MHz  = {0x00, 0xa3, 0x01};
            // static CanBitRateConfig constexpr BitRate_800kBPS_12MHz = {0x00, 0x00, 0x00}; // 12 mhz is to slow for this speed - not possible
            static CanBitRateConfig constexpr BitRate_1000kBPS_12MHz = {0x00, 0x88, 0x01};
        }

        MCP2515::MCP2515(cilo72::hw::SPIDevice &spi, const Oscillator oscillator, const Bitrate bitrate)
            : spi_(spi), oscillator_(oscillator), bitrate_(bitrate)
        {
            spi_.setFormat(8, SPI_CPOL_0, SPI_CPHA_0);
        }

        MCP2515::Error MCP2515::reset()
        {
            uint8_t tx[1] = {INSTRUCTION_RESET};
            uint8_t rx[1] = {0};
            spi_.xfer(tx, rx, sizeof(tx));

            sleep_ms(10);

            uint8_t zeros[14] = {0};
            setRegisters(Register::Txb0ctrl, zeros, sizeof(zeros));
            setRegisters(Register::Txb1ctrl, zeros, sizeof(zeros));
            setRegisters(Register::Txb2ctrl, zeros, sizeof(zeros));

            setRegister(Register::Rxb0ctrl, 0);
            setRegister(Register::Rxb1ctrl, 0);

            setRegister(Register::Caninte, CANINTF_RX0IF | CANINTF_RX1IF | CANINTF_ERRIF | CANINTF_MERRF);

            // receives all valid messages using either Standard or Extended Identifiers that
            // meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
            modifyRegister(Register::Rxb0ctrl,
                           RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
                           RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
            modifyRegister(Register::Rxb1ctrl,
                           RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
                           RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

            // clear filters and masks
            // do not filter any standard frames for RXF0 used by RXB0
            // do not filter any extended frames for RXF1 used by RXB1
            RXF filters[] = {RXF::RXF0, RXF::RXF1, RXF::RXF2, RXF::RXF3, RXF::RXF4, RXF::RXF5};
            for (int i = 0; i < 6; i++)
            {
                bool ext = (i == 1);
                Error result = setFilter(filters[i], ext, 0);
                if (result != Error::OK)
                {
                    return result;
                }
            }

            Mask masks[] = {Mask::Mask0, Mask::Mask1};
            for (int i = 0; i < 2; i++)
            {
                Error result = setFilterMask(masks[i], true, 0);
                if (result != Error::OK)
                {
                    return result;
                }
            }

            Error result = setBitrate(bitrate_);
            if (result == Error::OK)
            {
                result = setMode(Mode::Normal);
            }
            return result;
        }

        uint8_t MCP2515::readRegister(const Register reg) const
        {
            uint8_t tx[3] = {INSTRUCTION_READ, static_cast<uint8_t>(reg)};
            uint8_t rx[3] = {0};

            spi_.xfer(tx, rx, sizeof(tx));
            return rx[2];
        }

        void MCP2515::readRegisters(const Register reg, uint8_t values[], const uint8_t length) const
        {
            static constexpr size_t MAX_LENGTH = 20;
            uint8_t tx[MAX_LENGTH] = {0};
            uint8_t rx[MAX_LENGTH] = {0};

            tx[0] = INSTRUCTION_READ;
            tx[1] = toUnderlaying(reg);

            spi_.xfer(tx, rx, length + 2);

            for (size_t i = 0; i < length; i++)
            {
                values[i] = rx[i + 2];
            }
        }

        void MCP2515::setRegister(const Register reg, const uint8_t value) const
        {
            uint8_t tx[3] = {INSTRUCTION_WRITE, static_cast<uint8_t>(reg), value};
            uint8_t rx[3] = {0};

            spi_.xfer(tx, rx, sizeof(tx));
        }

        void MCP2515::setRegisters(const Register reg, const uint8_t values[], const uint8_t length) const
        {
            static constexpr size_t MAX_LENGTH = 20;
            uint8_t tx[MAX_LENGTH] = {INSTRUCTION_WRITE, static_cast<uint8_t>(reg)};
            uint8_t rx[MAX_LENGTH] = {0};

            for (size_t i = 0; i < length; i++)
            {
                tx[i + 2] = values[i];
            }

            spi_.xfer(tx, rx, length + 2);
        }

        void MCP2515::modifyRegister(const Register reg, const uint8_t mask, const uint8_t data) const
        {
            uint8_t tx[4] = {INSTRUCTION_BITMOD, static_cast<uint8_t>(reg), mask, data};
            uint8_t rx[4] = {0};

            spi_.xfer(tx, rx, sizeof(tx));
        }

        MCP2515::Error MCP2515::setFilterMask(const Mask mask, const bool ext, const uint32_t identifierMaskBits) const
        {
            Error res = setMode(Mode::Config);

            if (res != Error::OK)
            {
                return res;
            }

            uint8_t tbufdata[4];
            prepareId(tbufdata, ext, identifierMaskBits);

            Register reg;
            switch (mask)
            {
            case Mask::Mask0:
                reg = Register::Rxm0sidh;
                break;
            case Mask::Mask1:
                reg = Register::Rxm1sidh;
                break;
            default:
                return Error::FAIL;
            }

            setRegisters(reg, tbufdata, 4);

            return Error::OK;
        }

        MCP2515::Error MCP2515::setFilter(const RXF rxf, const bool ext, const uint32_t identifierFilterBits) const
        {
            Error res = setMode(Mode::Config);
            if (res != Error::OK)
            {
                return res;
            }

            Register reg;

            switch (rxf)
            {
            case RXF::RXF0:
                reg = Register::Rxf0sidh;
                break;
            case RXF::RXF1:
                reg = Register::Rxf1sidh;
                break;
            case RXF::RXF2:
                reg = Register::Rxf2sidh;
                break;
            case RXF::RXF3:
                reg = Register::Rxf3sidh;
                break;
            case RXF::RXF4:
                reg = Register::Rxf4sidh;
                break;
            case RXF::RXF5:
                reg = Register::Rxf5sidh;
                break;
            default:
                return Error::FAIL;
            }

            uint8_t tbufdata[4];
            prepareId(tbufdata, ext, identifierFilterBits);
            setRegisters(reg, tbufdata, 4);

            return Error::OK;
        }

        MCP2515::Error MCP2515::setMode(const Mode mode) const
        {
            modifyRegister(Register::Canctrl, CANCTRL_REQOP, toUnderlaying(mode));

            unsigned long endTime = to_ms_since_boot(get_absolute_time()) + 10;
            bool modeMatch = false;
            while (to_ms_since_boot(get_absolute_time()) < endTime)
            {
                uint8_t newmode = readRegister(Register::Canstat);
                newmode &= CANSTAT_OPMOD;

                modeMatch = static_cast<Mode>(newmode) == mode;

                if (modeMatch)
                {
                    break;
                }
            }

            return modeMatch  ? Error::OK : Error::FAIL;
        }

        MCP2515::Error MCP2515::setBitrate(const Bitrate bitrate)
        {
            Error error = setMode(Mode::Config);
            if (error != Error::OK)
            {
                return error;
            }

            CanBitRateConfig canBitRateConfig;

            switch (oscillator_)
            {
            case Oscillator::F_16MHZ:
                switch (bitrate)
                {
                case Bitrate::B_10KBPS:
                    canBitRateConfig = BitRate_10kBPS_16MHz;
                    break;

                case Bitrate::B_20KBPS:
                    canBitRateConfig = BitRate_20kBPS_16MHz;
                    break;

                case Bitrate::B_100KBPS:
                    canBitRateConfig = BitRate_100kBPS_16MHz;
                    break;

                case Bitrate::B_125KBPS:
                    canBitRateConfig = BitRate_125kBPS_16MHz;
                    break;

                case Bitrate::B_250KBPS:
                    canBitRateConfig = BitRate_250kBPS_16MHz;
                    break;

                case Bitrate::B_500KBPS:
                    canBitRateConfig = BitRate_500kBPS_16MHz;
                    break;

                case Bitrate::B_800KBPS:
                    canBitRateConfig = BitRate_800kBPS_16MHz;
                    break;

                case Bitrate::B_1000KBPS:
                    canBitRateConfig = BitRate_1000kBPS_16MHz;
                    break;

                default:
                    return Error::FAIL;
                }
                break;

            case Oscillator::F_12MHZ:
                switch (bitrate)
                {
                case Bitrate::B_10KBPS:
                    canBitRateConfig = BitRate_10kBPS_12MHz;
                    break;

                case Bitrate::B_20KBPS:
                    canBitRateConfig = BitRate_20kBPS_12MHz;
                    break;

                case Bitrate::B_100KBPS:
                    canBitRateConfig = BitRate_100kBPS_12MHz;
                    break;

                case Bitrate::B_125KBPS:
                    canBitRateConfig = BitRate_125kBPS_12MHz;
                    break;

                case Bitrate::B_250KBPS:
                    canBitRateConfig = BitRate_250kBPS_12MHz;
                    break;

                case Bitrate::B_500KBPS:
                    canBitRateConfig = BitRate_500kBPS_12MHz;
                    break;

                case Bitrate::B_1000KBPS:
                    canBitRateConfig = BitRate_1000kBPS_12MHz;
                    break;

                default:
                    return Error::FAIL;
                }
                break;

            case Oscillator::F_10MHZ:
                switch (bitrate)
                {
                case Bitrate::B_10KBPS:
                    canBitRateConfig = BitRate_10kBPS_10MHz;
                    break;

                case Bitrate::B_20KBPS:
                    canBitRateConfig = BitRate_20kBPS_10MHz;
                    break;

                case Bitrate::B_100KBPS:
                    canBitRateConfig = BitRate_100kBPS_10MHz;
                    break;

                case Bitrate::B_125KBPS:
                    canBitRateConfig = BitRate_125kBPS_10MHz;
                    break;

                case Bitrate::B_250KBPS:
                    canBitRateConfig = BitRate_250kBPS_10MHz;
                    break;

                case Bitrate::B_500KBPS:
                    canBitRateConfig = BitRate_500kBPS_10MHz;
                    break;

                case Bitrate::B_1000KBPS:
                    canBitRateConfig = BitRate_1000kBPS_10MHz;
                    break;

                default:
                    return Error::FAIL;
                }
                break;

            case Oscillator::F_8MHZ:
                switch (bitrate)
                {
                case Bitrate::B_10KBPS:
                    canBitRateConfig = BitRate_10kBPS_8MHz;
                    break;

                case Bitrate::B_20KBPS:
                    canBitRateConfig = BitRate_20kBPS_8MHz;
                    break;

                case Bitrate::B_100KBPS:
                    canBitRateConfig = BitRate_100kBPS_8MHz;
                    break;

                case Bitrate::B_125KBPS:
                    canBitRateConfig = BitRate_125kBPS_8MHz;
                    break;

                case Bitrate::B_250KBPS:
                    canBitRateConfig = BitRate_250kBPS_8MHz;
                    break;

                case Bitrate::B_500KBPS:
                    canBitRateConfig = BitRate_500kBPS_8MHz;
                    break;

                case Bitrate::B_800KBPS:
                    canBitRateConfig = BitRate_800kBPS_8MHz;
                    break;

                case Bitrate::B_1000KBPS:
                    canBitRateConfig = BitRate_1000kBPS_8MHz;
                    break;

                default:
                    return Error::FAIL;
                }
                break;

            default:
                return Error::FAIL;
            }

            setRegister(Register::Cnf1, canBitRateConfig.cnf1);
            setRegister(Register::Cnf2, canBitRateConfig.cnf2);
            setRegister(Register::Cnf3, canBitRateConfig.cnf3);
            return Error::OK;
        }

        void MCP2515::prepareId(uint8_t *buffer, const bool ext, const uint32_t id) const
        {

            uint16_t canid = (uint16_t)(id & 0x0FFFF);

            if (ext)
            {
                buffer[MCP_EID0] = (uint8_t)(canid & 0xFF);
                buffer[MCP_EID8] = (uint8_t)(canid >> 8);
                canid = (uint16_t)(id >> 16);
                buffer[MCP_SIDL] = (uint8_t)(canid & 0x03);
                buffer[MCP_SIDL] += (uint8_t)((canid & 0x1C) << 3);
                buffer[MCP_SIDL] |= TXB_EXIDE_MASK;
                buffer[MCP_SIDH] = (uint8_t)(canid >> 5);
            }
            else
            {
                buffer[MCP_SIDH] = (uint8_t)(canid >> 3);
                buffer[MCP_SIDL] = (uint8_t)((canid & 0x07) << 5);
                buffer[MCP_EID0] = 0;
                buffer[MCP_EID8] = 0;
            }
        }

        MCP2515::Error MCP2515::sendMessage(const TXBn txbn, const cilo72::core::CanMessage &message) const
        {

            if (message.dlc() > CAN_MAX_DLEN)
            {
                return Error::FAILTX;
            }

            TXBn_REGS txbuf = txbn_regs(txbn);

            uint8_t data[13];

            bool ext = message.extended();
            bool rtr = message.rtr();
            uint32_t id = message.id();

            prepareId(data, ext, id);

            data[MCP_DLC] = rtr ? (message.dlc() | RTR_MASK) : message.dlc();

            memcpy(&data[MCP_DATA], message.data(), message.dlc());

            setRegisters(txbuf.SIDH, data, 5 + message.dlc());

            modifyRegister(txbuf.CTRL, TXB_TXREQ, TXB_TXREQ);

            uint8_t ctrl = readRegister(txbuf.CTRL);
            if ((ctrl & (TXB_ABTF | TXB_MLOA | TXB_TXERR)) != 0)
            {
                return Error::FAILTX;
            }
            return Error::OK;
        }

        MCP2515::Error MCP2515::sendMessage(const cilo72::core::CanMessage &message) const
        {
            if (message.dlc() > CAN_MAX_DLEN)
            {
                return Error::FAILTX;
            }

            static constexpr size_t N_TXBUFFERS = 3;
            TXBn txBuffers[N_TXBUFFERS] = {TXBn::TXB0, TXBn::TXB1, TXBn::TXB2};

            for (size_t i = 0; i < N_TXBUFFERS; i++)
            {
                TXBn_REGS txbuf = txbn_regs(txBuffers[i]);
                uint8_t ctrlval = readRegister(txbuf.CTRL);
                if ((ctrlval & TXB_TXREQ) == 0)
                {
                    return sendMessage(txBuffers[i], message);
                }
            }

            return Error::ALLTXBUSY;
        }

        MCP2515::Error MCP2515::readMessage(const RXBn rxbn, cilo72::core::CanMessage &message)
        {
            RXBn_REGS rxb = rxbn_regs(rxbn);// &RXB[toUnderlaying(rxbn)];

            uint8_t tbufdata[5];

            readRegisters(rxb.SIDH, tbufdata, 5);

            uint32_t id = (tbufdata[MCP_SIDH] << 3) + (tbufdata[MCP_SIDL] >> 5);

            if ((tbufdata[MCP_SIDL] & TXB_EXIDE_MASK) == TXB_EXIDE_MASK)
            {
                id = (id << 2) + (tbufdata[MCP_SIDL] & 0x03);
                id = (id << 8) + tbufdata[MCP_EID8];
                id = (id << 8) + tbufdata[MCP_EID0];
                message.setExtended(true);
            }

            uint8_t dlc = (tbufdata[MCP_DLC] & DLC_MASK);
            if (dlc > CAN_MAX_DLEN)
            {
                return Error::FAIL;
            }

            uint8_t ctrl = readRegister(rxb.CTRL);
            if (ctrl & RXBnCTRL_RTR)
            {
                message.setRtr(true);
            }

            message.setId(id);
            message.setDlc(dlc);

            readRegisters(rxb.DATA, message.data(), dlc);

            modifyRegister(Register::Canintf, toUnderlaying(rxb.RXnIF), 0);

            return Error::OK;
        }

        MCP2515::Error MCP2515::readMessage(cilo72::core::CanMessage &message)
        {
            Error rc;
            uint8_t stat = getStatus();

            if (stat & STAT_RX0IF)
            {
                rc = readMessage(RXBn::RXB0, message);
            }
            else if (stat & STAT_RX1IF)
            {
                rc = readMessage(RXBn::RXB1, message);
            }
            else
            {
                rc = Error::NOMSG;
            }

            return rc;
        }

        bool MCP2515::checkReceive(void) const
        {
            uint8_t res = getStatus();
            if (res & STAT_RXIF_MASK)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        uint8_t MCP2515::getStatus(void) const
        {

            uint8_t tx[2] = {INSTRUCTION_READ_STATUS, 0};
            uint8_t rx[2] = {0};

            spi_.xfer(tx, rx, sizeof(tx));

            return rx[1];
        }


        MCP2515::TXBn_REGS MCP2515::txbn_regs(TXBn n) const
        {
            switch(n)
            {
                case TXBn::TXB0: return {MCP2515::Register::Txb0ctrl, MCP2515::Register::Txb0sidh, MCP2515::Register::Txb0data};
                case TXBn::TXB1: return {MCP2515::Register::Txb1ctrl, MCP2515::Register::Txb1sidh, MCP2515::Register::Txb1data};
                case TXBn::TXB2: return {MCP2515::Register::Txb2ctrl, MCP2515::Register::Txb2sidh, MCP2515::Register::Txb2data};
            }

            assert(0);
            return {MCP2515::Register::Txb0ctrl, MCP2515::Register::Txb0sidh, MCP2515::Register::Txb0data};
        }

        MCP2515::RXBn_REGS MCP2515::rxbn_regs(RXBn n) const
        {
            switch(n)
            {
                case RXBn::RXB0: return {MCP2515::Register::Rxb0ctrl, MCP2515::Register::Rxb0sidh, MCP2515::Register::Rxb0data, MCP2515::CANINTF_RX0IF};
                case RXBn::RXB1: return {MCP2515::Register::Rxb1ctrl, MCP2515::Register::Rxb1sidh, MCP2515::Register::Rxb1data, MCP2515::CANINTF_RX1IF};
            }
            assert(0);
            return {MCP2515::Register::Rxb0ctrl, MCP2515::Register::Rxb0sidh, MCP2515::Register::Rxb0data, MCP2515::CANINTF_RX0IF};
        }
    }
}
