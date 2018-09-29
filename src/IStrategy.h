/*
   bully
   Copyright (C) 2018  Philippe Desrosiers

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BULLY_I_STRATEGY_H__
#define __BULLY_I_STRATEGY_H__

#include <Arduino.h>
#include <MotorController.h>
#include <IRSensor.h>
#include <LineSensor.h>

class Strategy
{
  public:
    Strategy(MotorController motor_controller, IRSensor sensor_front, IRSensor sensor_left, IRSensor sensor_right, LineSensor ls_left, LineSensor ls_right) {}
    virtual ~Strategy() {}
    virtual void onTargetLost() = 0;
    virtual void onTargetDetect() = 0;
    virtual void onLineDetect() = 0;
    virtual void onMovementEnd() = 0;
};

#endif //__BULLY_I_STRATEGY_H__