/*
  Copyright (c) 2024 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#pragma once

namespace cilo72
{
  namespace core
  {
    class State;

    /*!
     * \brief StateMachineCommand
      * This class represents a command that can be executed by a state machine.
     */
    class StateMachineCommand
    {
    public:
      enum class Type
      {
        Nothing,
        Change
      };

      /*!
       * \brief StateMachineCommand
       * \param type Type of the command
       */
      StateMachineCommand(Type type = Type::Nothing)
          : type_(type)
      {
      }

      /*!
       * \brief type
       * \return Type of the command
       */
      Type type() const
      {
        return type_;
      }

    protected:
      Type type_;
    };

    /*!
     * \brief StateMachineCommandNothing
     * This class represents the next state machine command to execute.
     */
    class StateMachineCommandChange : public StateMachineCommand
    {
    public:
      /*!
        * \brief StateMachineCommandNothing
        */
      StateMachineCommandChange()
          : StateMachineCommand(Type::Change)
      {
      }

      /*!
       * \brief to
       * \param to Next state
       */ 
      void to(State *to)
      {
        to_ = to;
      }

      /*!
       * \brief state
       * \return Next state
       */ 
      State *state() const
      {
        return to_;
      }

protected:
      State *to_;
    };
  }
}