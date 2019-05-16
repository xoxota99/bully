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
    int newVal = 0;
    for (int i = 0; i < _smoothing; i++)
    {
        newVal += analogRead(_pin);
    }
    newVal = newVal / _smoothing;
    if (_op_mode == DISCRETE)
    {
        _analog_value = newVal;
        if (newVal > _high_threshold && _digital_value != HIGH)
        {
            _digital_value = HIGH;
            if (_callback != 0)
                _callback(_pin, _digital_value);
        }
        else if (newVal <= _low_threshold && _digital_value != LOW)
        {
            _digital_value = LOW;
            if (_callback != 0)
                _callback(_pin, _digital_value);
        }
    }
    else if (_op_mode == CONTINUOUS && newVal != _analog_value)
    {
        _analog_value = newVal;
        if (_callback != 0)
            _callback(_pin, _analog_value);
    }
}

int LineSensor::getAnalogValue() { return _analog_value; }
bool LineSensor::getDigitalValue()
{
    return _digital_value;
}

op_mode_t LineSensor::getOpMode() { return _op_mode; }
