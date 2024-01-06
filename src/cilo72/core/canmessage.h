/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include <stdint.h>
#include <string.h>

namespace cilo72
{
    namespace core
    {
        /*!
        * \brief CAN message structure
        */
        class CanMessage
        {
            public:
            /*! Maximum data length in bytes */            
            static constexpr size_t MAX_DATA_LEN = 8;

            /*! CAN frame format */
            enum class Frame
            {
                Standard,   /*!< Standard frame */
                Extended    /*!< Extended frame */
            };

            /*! \brief Constructor
            */
            CanMessage()
            : id_(0)
            , dlc_(0)
            , extended_(false)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
            }

            /*! \brief Constructor
            * \param id CAN message ID
            * \param frame CAN frame format
            */  
            CanMessage(uint32_t id, Frame frame)
            : id_(id)
            , dlc_(0)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
            }

            /*! \brief Constructs a can message with one data byte
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0)
            : id_(id)
            , dlc_(1)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
            }

            /*! \brief Constructs a can message with two data bytes
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            * \param b1 Data byte 1
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0, uint8_t b1)
            : id_(id)
            , dlc_(2)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
                data_[1] = b1;
            }

            /*! \brief Constructs a can message with three data bytes
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            * \param b1 Data byte 1
            * \param b2 Data byte 2
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0, uint8_t b1, uint8_t b2)
            : id_(id)
            , dlc_(3)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
                data_[1] = b1;
                data_[2] = b2;
            }

            /*! \brief Constructs a can message with four data bytes
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            * \param b1 Data byte 1
            * \param b2 Data byte 2
            * \param b3 Data byte 3
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
            : id_(id)
            , dlc_(4)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
                data_[1] = b1;
                data_[2] = b2;
                data_[3] = b3;
            }

            /*! \brief Constructs a can message with five data bytes
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            * \param b1 Data byte 1
            * \param b2 Data byte 2
            * \param b3 Data byte 3
            * \param b4 Data byte 4
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
            : id_(id)
            , dlc_(5)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
                data_[1] = b1;
                data_[2] = b2;
                data_[3] = b3;
                data_[4] = b4;
            }

            /*! \brief Constructs a can message with six data bytes
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            * \param b1 Data byte 1
            * \param b2 Data byte 2
            * \param b3 Data byte 3
            * \param b4 Data byte 4
            * \param b5 Data byte 5
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5)
            : id_(id)
            , dlc_(6)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
                data_[1] = b1;
                data_[2] = b2;
                data_[3] = b3;
                data_[4] = b4;
                data_[5] = b5;
            }

            /*! \brief Constructs a can message with seven data bytes
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            * \param b1 Data byte 1
            * \param b2 Data byte 2
            * \param b3 Data byte 3
            * \param b4 Data byte 4
            * \param b5 Data byte 5
            * \param b6 Data byte 6
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6)
            : id_(id)
            , dlc_(7)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
                data_[1] = b1;
                data_[2] = b2;
                data_[3] = b3;
                data_[4] = b4;
                data_[5] = b5;
                data_[6] = b6;
            }

            /*! \brief Constructs a can message with eight data bytes
            * \param id CAN message ID
            * \param frame CAN frame format
            * \param b0 Data byte 0
            * \param b1 Data byte 1
            * \param b2 Data byte 2
            * \param b3 Data byte 3
            * \param b4 Data byte 4
            * \param b5 Data byte 5
            * \param b6 Data byte 6
            * \param b7 Data byte 7
            */
            CanMessage(uint32_t id, Frame frame, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7)
            : id_(id)
            , dlc_(8)
            , extended_(frame == Frame::Extended)
            , rtr_(false)
            {
                memset(data_, 0, MAX_DATA_LEN);
                data_[0] = b0;
                data_[1] = b1;
                data_[2] = b2;
                data_[3] = b3;
                data_[4] = b4;
                data_[5] = b5;
                data_[6] = b6;
                data_[7] = b7;
            }

            /*!
            * Get the data length code.
            * \return Data length code
            */
            uint32_t dlc() const { return dlc_; }

            /*!
            * Set the data length code.
            * \param value Data length code
            */
            void setDlc(uint32_t value) { dlc_ = value; }

            /*!
            * Get the CAN message ID.
            * \return CAN message ID
            */
            uint32_t id() const { return id_; }

            /*!
            * Set the CAN message ID.
            * \param value CAN message ID
            */
            void setId(uint32_t value) { id_ = value; }

            /*!
            * Get the CAN frame format.
            * \return CAN frame format
            */
            bool extended() const { return extended_; }

            /*!
            * Set the CAN frame format.
            * \param value CAN frame format
            */
            void setExtended(bool value) { extended_ = value; }

            /*!
            * Get the remote transmission request flag.
            * \return Remote transmission request flag
            */
            bool rtr() const { return rtr_; }

            /*!
            * Set the remote transmission request flag.
            * \param value Remote transmission request flag
            */
            void setRtr(bool value) { rtr_ = value; }

            /*!
            * Get the data bytes.
            * \return Data bytes
            */
            const uint8_t * data() const { return data_; }


            /*!
            * Get the data bytes.
            * \return Data bytes
            */
            uint8_t * data() { return data_; }

            /*!
            * Overload [] operator to access data directly.
            * \param index Index of the data to access
            * \return Data at the given index
            */
            uint8_t& operator[](size_t index) {
                if (index >= MAX_DATA_LEN) {
                    assert(0);
                }
                return data_[index];
            }

            /*!
            * Overload [] operator to access data directly (const version).
            * \param index Index of the data to access
            * \return Data at the given index
            */
            const uint8_t& operator[](size_t index) const {
                if (index >= MAX_DATA_LEN) {
                    assert(0);
                }
                return data_[index];
            }

        protected:
            uint32_t id_;
            uint32_t dlc_;
            uint8_t data_[MAX_DATA_LEN] __attribute__((aligned(8)));
            bool extended_;
            bool rtr_;
        };
    }
}
