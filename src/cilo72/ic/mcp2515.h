/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include "pico/stdlib.h"
#include "cilo72/hw/spi_device.h"
#include "cilo72/core/canmessage.h"

namespace cilo72
{
    namespace ic
    {
        /*!
        * \brief MCP2515 CAN Controller
        *      This class implements the MCP2515 CAN Controller
        *     The MCP2515 is connected via SPI.
        *    The SPI bus is shared with other devices.
        */
        class MCP2515
        {
        public:
            /*!
            * \brief Error codes
            *      This enum defines the error codes of the MCP2515
            *    The error codes are returned by the functions of the MCP2515 class
            */
            enum class Error
            {
                OK = 0,
                FAIL = 1,
                ALLTXBUSY = 2,
                FAILINIT = 3,
                FAILTX = 4,
                NOMSG = 5
            };

            /*!
            * \brief Oscillator
            *      This enum defines the oscillator of the MCP2515
            *    The oscillator is used to set the clock frequency of the MCP2515   
            */
            enum class Oscillator
            {
                F_16MHZ,    //< 16 MHz
                F_12MHZ,    //< 12 MHz 
                F_10MHZ,    //< 10 MHz
                F_8MHZ      //< 8 MHz
            };

            /*!
            * \brief Bitrate
            *      This enum defines the bitrate of the MCP2515
            *   The bitrate is used to set the CAN bus speed
            */
            enum class Bitrate
            {
                B_10KBPS,   //< 10 kbps
                B_20KBPS,   //< 20 kbps
                B_100KBPS,  //< 100 kbps
                B_125KBPS,  //< 125 kbps
                B_250KBPS,  //< 250 kbps
                B_500KBPS,  //< 500 kbps
                B_800KBPS,  //< 800 kbps
                B_1000KBPS  //< 1000 kbps
            };

            /*!
            * \brief Mode
            *      This enum defines the mode of the MCP2515
            *  The mode is used to set the mode of the MCP2515
            */
            enum class Mode : uint8_t
            {
                Normal     = 0x00,  //< Normal mode (default after power-up)
                Sleep      = 0x20,  //< Sleep mode
                Loopback   = 0x40,  //< Loopback mode
                Listenonly = 0x60,  //< Listen-only mode
                Config     = 0x80,  //< Configuration mode
                Powerup    = 0xe0   //< Power-up mode
            };
            
            /*!
            * \brief RXF
            *      This enum defines the RXF (receive filters) of the MCP2515
            * The RXF is used to set the RXF of the MCP2515
            */
            enum class RXF : uint8_t
            {
                RXF0 = 0,   //< RXF0
                RXF1 = 1,   //< RXF1
                RXF2 = 2,   //< RXF2
                RXF3 = 3,   //< RXF3
                RXF4 = 4,   //< RXF4
                RXF5 = 5    //< RXF5
            };

            /*!
            * \brief Mask
            *      This enum defines the Mask (receive masks) of the MCP2515
            */
            enum class Mask
            {
                Mask0,  //< Mask0
                Mask1   //< Mask1
            };

            /*!
            * \brief Constructor
            *      This constructor creates a MCP2515 object
            * \param spi SPI device
            * \param oscillator Oscillator used to set the clock frequency of the MCP2515
            * \param bitrate Bitrate of the CAN bus
            */
            MCP2515(cilo72::hw::SPIDevice &spi, const Oscillator oscillator, const Bitrate bitrate);

            /*!
            * \brief Reset
            *      This function resets the MCP2515 and initializes the CAN controller.
            */
            Error reset();

            /*!
            * \brief Set bitrate of the CAN bus
            *      This function sets the bitrate of the CAN bus
            * \param canSpeed Bitrate of the CAN bus
            * \return Error code
            */
            Error setBitrate(const Bitrate canSpeed);

            /*!
            * \brief Set mode
            *      This function sets the mode of the MCP2515
            * \param mode Mode of the MCP2515
            * \return Error code
            */
            Error setMode(const Mode mode) const;

            /*!
            * \brief Set filter
            *      This function sets the filter of the MCP2515
            * \param rxf RXF (receive filters) of the MCP2515
            * \param ext Extended frame
            * \param identifierFilterBits Identifier filter bits
            * \return Error code
            */
            Error setFilter(const RXF rxf, const bool ext, const uint32_t identifierFilterBits) const;

            /*!
            * \brief Set filter mask
            *      This function sets the filter mask of the MCP2515
            * \param mask Mask (receive masks) of the MCP2515
            * \param ext Extended frame
            * \param identifierMaskBits Identifier mask bits
            * \return Error code
            */
            Error setFilterMask(const Mask mask, const bool ext, const uint32_t identifierMaskBits) const;

            /*!
            * \brief Send message
            *      This function sends a CAN message
            * \param message CAN message
            * \return Error code
            */
            Error sendMessage(const cilo72::core::CanMessage &message) const;

            /*!
            * \brief Read message
            *      This function reads a CAN message
            * \param message CAN message
            * \return Error code
            */
            Error readMessage(cilo72::core::CanMessage & message);

            /*!
            * \brief Get status
            *      This function returns the status of the MCP2515
            * \return Status
            */
            uint8_t getStatus(void) const;

            /*!
            * \brief Check receive
            *      This function checks if a CAN message is available
            * \return True if a CAN message is available, otherwise false
            */
            bool checkReceive(void) const;
        protected:
            cilo72::hw::SPIDevice &spi_;
            const Oscillator oscillator_;
            const Bitrate bitrate_;

            enum class TXBn 
            {
                TXB0 = 0,
                TXB1 = 1,
                TXB2 = 2
            };

            enum class RXBn 
            {
                RXB0 = 0,
                RXB1 = 1
            };


            enum class Register : uint8_t
            {
                Rxf0sidh = 0x00,
                Rxf0sidl = 0x01,
                Rxf0eid8 = 0x02,
                Rxf0eid0 = 0x03,
                Rxf1sidh = 0x04,
                Rxf1sidl = 0x05,
                Rxf1eid8 = 0x06,
                Rxf1eid0 = 0x07,
                Rxf2sidh = 0x08,
                Rxf2sidl = 0x09,
                Rxf2eid8 = 0x0a,
                Rxf2eid0 = 0x0b,
                Canstat = 0x0e,
                Canctrl = 0x0f,
                Rxf3sidh = 0x10,
                Rxf3sidl = 0x11,
                Rxf3eid8 = 0x12,
                Rxf3eid0 = 0x13,
                Rxf4sidh = 0x14,
                Rxf4sidl = 0x15,
                Rxf4eid8 = 0x16,
                Rxf4eid0 = 0x17,
                Rxf5sidh = 0x18,
                Rxf5sidl = 0x19,
                Rxf5eid8 = 0x1a,
                Rxf5eid0 = 0x1b,
                Tec = 0x1c,
                Rec = 0x1d,
                Rxm0sidh = 0x20,
                Rxm0sidl = 0x21,
                Rxm0eid8 = 0x22,
                Rxm0eid0 = 0x23,
                Rxm1sidh = 0x24,
                Rxm1sidl = 0x25,
                Rxm1eid8 = 0x26,
                Rxm1eid0 = 0x27,
                Cnf3 = 0x28,
                Cnf2 = 0x29,
                Cnf1 = 0x2a,
                Caninte = 0x2b,
                Canintf = 0x2c,
                Eflg = 0x2d,
                Txb0ctrl = 0x30,
                Txb0sidh = 0x31,
                Txb0sidl = 0x32,
                Txb0eid8 = 0x33,
                Txb0eid0 = 0x34,
                Txb0dlc = 0x35,
                Txb0data = 0x36,
                Txb1ctrl = 0x40,
                Txb1sidh = 0x41,
                Txb1sidl = 0x42,
                Txb1eid8 = 0x43,
                Txb1eid0 = 0x44,
                Txb1dlc = 0x45,
                Txb1data = 0x46,
                Txb2ctrl = 0x50,
                Txb2sidh = 0x51,
                Txb2sidl = 0x52,
                Txb2eid8 = 0x53,
                Txb2eid0 = 0x54,
                Txb2dlc = 0x55,
                Txb2data = 0x56,
                Rxb0ctrl = 0x60,
                Rxb0sidh = 0x61,
                Rxb0sidl = 0x62,
                Rxb0eid8 = 0x63,
                Rxb0eid0 = 0x64,
                Rxb0dlc = 0x65,
                Rxb0data = 0x66,
                Rxb1ctrl = 0x70,
                Rxb1sidh = 0x71,
                Rxb1sidl = 0x72,
                Rxb1eid8 = 0x73,
                Rxb1eid0 = 0x74,
                Rxb1dlc = 0x75,
                Rxb1data = 0x76
            };

            struct TXBn_REGS
            {
                Register CTRL;
                Register SIDH;
                Register DATA;
            };

           enum Canintf : uint8_t
            {
                CANINTF_RX0IF = 0x01,
                CANINTF_RX1IF = 0x02,
                CANINTF_TX0IF = 0x04,
                CANINTF_TX1IF = 0x08,
                CANINTF_TX2IF = 0x10,
                CANINTF_ERRIF = 0x20,
                CANINTF_WAKIF = 0x40,
                CANINTF_MERRF = 0x80
            };

            struct RXBn_REGS
            {
                Register CTRL;
                Register SIDH;
                Register DATA;
                Canintf RXnIF;
            };         

            void setRegister(const Register reg, const uint8_t value) const;
            void setRegisters(const Register reg, const uint8_t values[], const uint8_t length) const;
            uint8_t readRegister(const Register reg) const;
            void readRegisters(const Register reg, uint8_t values[], const uint8_t length) const;
            void modifyRegister(const Register reg, const uint8_t mask, const uint8_t data) const;
            Error sendMessage(const TXBn txbn, const cilo72::core::CanMessage & message) const;
            Error readMessage(const RXBn rxbn, cilo72::core::CanMessage & message);
            void prepareId(uint8_t *buffer, const bool ext, const uint32_t id) const;
            TXBn_REGS txbn_regs(TXBn n) const;
            RXBn_REGS rxbn_regs(RXBn n) const;
        };
    }
}