#include "LineSensor.h"

LineSensor::LineSensor(int pin, int low_threshold, int high_threshold,
                       int smoothing, op_mode_t op_mode,
                       sense_callback_t callback)
{
    _pin = pin;
    _smoothing = smoothing;
    _op_mode = op_mode;
    _callback = callback;
    _low_threshold = low_threshold;
    _high_threshold = high_threshold;
}

void LineSensor::loop()
{
    if (_attached)
    {
        int newVal = 0;
        for (int i = 0; i < _smoothing; i++)
        {
            newVal += analogRead(_pin);
        }
        newVal = newVal / _smoothing;

        if (_op_mode == DISCRETE)
        {
            if (newVal > _high_threshold && _value != MAX_VALUE)
            {
                _value = MAX_VALUE;
                if (_callback != 0)
                    _callback(_pin, _value);
            }
            else if (newVal <= _low_threshold && _value != MIN_VALUE)
            {
                _value = MIN_VALUE;
                if (_callback != 0)
                    _callback(_pin, _value);
            }
        }
        else if (_op_mode == CONTINUOUS && newVal != _value)
        {
            _value = newVal;
            if (_callback != 0)
                _callback(_pin, _value);
        }
    }
}

int LineSensor::getValue() { return _value; }

op_mode_t LineSensor::getOpMode() { return _op_mode; }
