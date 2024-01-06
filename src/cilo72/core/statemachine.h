/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

#include "state.h"
#include "statemachinecommand.h"

namespace cilo72
{
  namespace core
  {
    /*!
     * \brief StateMachine
     */
    class StateMachine
    {
    public:
      /*!
       * \brief StateMachine
       * \param state Initial state
       */
      StateMachine(State *state);

      /*!
       * \brief run  Run the state machine
       * This method must be called in a loop.
       */
      void run();
    private:
      State *state_;
    };
  }
}