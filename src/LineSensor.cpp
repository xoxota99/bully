#include "LineSensor.h"

LineSensor LineSensor::attach(int pin, sense_callback_t callback)
{
    LineSensor ls;
    ls._pin = pin;
    ls._callback = callback;
    return ls;
}

LineSensor LineSensor::attach(int pin, int smoothing, op_mode_t op_mode,
                              sense_callback_t callback)
{
    LineSensor ls = attach(pin, callback);
    ls._smoothing = smoothing;
    ls._op_mode = op_mode;
    return ls;
}

LineSensor LineSensor::attach(int pin, int low_threshold, int high_threshold,
                              int smoothing, op_mode_t op_mode,
                              sense_callback_t callback)
{
    LineSensor ls = attach(pin, smoothing, op_mode, callback);
    ls._low_threshold = low_threshold;
    ls._high_threshold = high_threshold;
    return ls;
}

void LineSensor::detach() { _attached = false; }

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
                _callback(_value, _pin);
            }
            else if (newVal <= _low_threshold && _value != MIN_VALUE)
            {
                _value = MIN_VALUE;
                _callback(_value, _pin);
            }
        }
        else if (_op_mode == CONTINUOUS && newVal != _value)
        {
            _value = newVal;
            _callback(_value, _pin);
        }
    }
}

int LineSensor::getValue() { return _value; }

op_mode_t LineSensor::getOpMode() { return _op_mode; }
