/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include "statemachine.h"

namespace cilo72
{
    namespace core
    {

        StateMachine::StateMachine(State *state)
            : state_(state)
        {
            state_->onEnter();
        }

        void StateMachine::run()
        {
            const StateMachineCommand *cmd = state_->run();

            switch (cmd->type())
            {
            case StateMachineCommand::Type::Nothing:
            {
            }
            break;

            case StateMachineCommand::Type::Change:
            {
                const StateMachineCommandChange *next = static_cast<const StateMachineCommandChange *>(cmd);

                State *nextState = next->state();

                if (nextState)
                {
                    if (nextState != state_)
                    {
                        state_->onExit();
                        nextState->onEnter();
                        state_ = nextState;
                    }
                }
            }
            break;
            }
        }

        void StateMachine::changeState(State *state)
        {
            state_->onExit();
            state->onEnter();
            state_ = state;
        }

        bool StateMachine::changeStateIfOther(State *state)
        {
            if(state != state_)
            {
                changeState(state);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}