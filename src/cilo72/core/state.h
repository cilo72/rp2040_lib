/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include <functional>
#include "statemachinecommand.h"

namespace cilo72
{
    namespace core
    {
        /*!
         * \brief State
         * This class represents a state in a state machine.
         */
        class State
        {
        public:
            /*!
             * \brief State
             */
            State()
                : onEnter_([]() {}), onRun_([&](State &state) -> const StateMachineCommand *
                                            { return nothing(); }),
                  onExit_([]() {})
            {
            }

            /*!
             * \brief run
             * \return The next command to execute
             */
            const StateMachineCommand *run()
            {
                return onRun_(*this);
            }

            /*!
             * \brief setOnRun
             * \param f Function to execute when the state is run
             */
            void setOnRun(std::function<const StateMachineCommand *(State &state)> f)
            {
                onRun_ = f;
            }
            
            /*!
             * \brief setOnEnter
             * \param f Function to execute when the state is entered
             */
            void setOnEnter(std::function<void()> f)
            {
                onEnter_ = f;
            }

            /*!
             * \brief onEnter
             */
            void onEnter()
            {
                onEnter_();
            }

            /*!
             * \brief setOnExit
             * \param f Function to execute when the state is exited
             */
            void setOnExit(std::function<void()> f)
            {
                onExit_ = f;
            }

            /*!
             * \brief onExit
             */
            void onExit()
            {
                onExit_();
            }

            /*!
             * \brief nothing
             * \return The state machine should not change the state
             */
            const StateMachineCommand *nothing() const
            {
                return &nothing_;
            }

            /*!
             * \brief changeTo
             * \param next The next state
             * \return The state machine should change to the next state
             */
            const StateMachineCommand *changeTo(State *next)
            {
                change_.to(next);
                return &change_;
            }

        protected:
            StateMachineCommand nothing_;
            StateMachineCommandChange change_;
            std::function<void()> onEnter_;
            std::function<const StateMachineCommand *(State &state)> onRun_;
            std::function<void()> onExit_;
        };
    }
}