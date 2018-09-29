/**
 * Generic support for NPN-style IR reflectance sensors.
 * Supports ACTIVE_LOW or ACTIVE_HIGH modes, callbacks on
 * rising / falling edge.
 * */

#ifndef __IR_SENSOR_H__
#define __IR_SENSOR_H__
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

#include <Arduino.h>
#include <iotypes.h>

typedef void (*target_acquire_callback_t)(int);
typedef void (*target_lost_callback_t)(int);

class IRSensor
{
  private:
    int _pin;
    target_acquire_callback_t _target_acquired_callback = 0;
    target_lost_callback_t _target_lost_callback = 0;
    io_mode_t _io_mode = ACTIVE_HIGH;
    bool _target_acquired = false;

    IRSensor(){};

  public:
    IRSensor(int pin,
             io_mode_t ioMode,
             target_acquire_callback_t target_acquired_callback,
             target_lost_callback_t target_lost_callback);

    IRSensor(int pin,
             target_acquire_callback_t target_acquired_callback,
             target_lost_callback_t target_lost_callback) : IRSensor(pin,
                                                                     ACTIVE_HIGH,
                                                                     target_acquired_callback,
                                                                     target_lost_callback) {}
    void loop();
    bool isTargetAcquired();
};

#endif // __IR_SENSOR_H
