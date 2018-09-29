/**
 * Generic support for QTR Analog Line Sensors.
 * Supports callbacks on rising / falling edge.
 * */

#ifndef __LINE_SENSOR_H__
#define __LINE_SENSOR_H__
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

typedef void (*sense_callback_t)(int, int);

enum op_mode_t
{
    DISCRETE,  // only callback when the analog value moves past a threshold value.
    CONTINUOUS // callback whenever the analog value changes.
};

class LineSensor
{
  private:
    const int MAX_VALUE = 1023, MIN_VALUE = 0;
    int _pin;
    int _smoothing = 4;
    sense_callback_t _callback;
    int _analog_value;
    bool _digital_value = LOW;
    int _low_threshold = 450;
    int _high_threshold = 575;
    op_mode_t _op_mode = DISCRETE;

    LineSensor(){};

  public:
    LineSensor(int pin, sense_callback_t callback)
        : LineSensor(pin, 4, DISCRETE, callback) {}
    LineSensor(int pin, int smoothing, op_mode_t op_mode, sense_callback_t callback)
        : LineSensor(pin, 450, 575, smoothing, op_mode, callback) {}
    LineSensor(int pin, int low_threshold, int high_threshold, int smoothing, op_mode_t op_mode, sense_callback_t callback);
    void loop();
    int getAnalogValue();
    bool getDigitalValue();
    op_mode_t getOpMode();
};
#endif // __LINE_SENSOR_H
