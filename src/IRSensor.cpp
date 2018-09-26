#include "IRSensor.h"

IRSensor IRSensor::attach(int pin, target_acquire_callback_t target_acquired_callback, target_lost_callback_t target_lost_callback)
{
    return IRSensor::attach(pin, ACTIVE_HIGH, target_acquired_callback, target_lost_callback);
}

IRSensor IRSensor::attach(int pin, io_mode_t ioMode, target_acquire_callback_t target_acquired_callback, target_lost_callback_t target_lost_callback)
{
    IRSensor ir;
    ir._pin = pin;
    ir._target_acquired_callback = target_acquired_callback;
    ir._target_lost_callback = target_lost_callback;
    ir._io_mode = ioMode;

    return ir;
}

void IRSensor::detach()
{
    _attached = false;
}

void IRSensor::loop()
{
    if (_attached)
    {
        //do some stuff, make some callbacks.
        int val = digitalReadFast(_pin);
        if ((val == HIGH && _io_mode == ACTIVE_HIGH) ||
            (val == LOW && _io_mode == ACTIVE_LOW))
        {
            if (!_target_acquired)
            {
                _target_acquired = true;
                if (_target_acquired_callback != 0)
                {
                    _target_acquired_callback(_pin);
                }
            }
            else
            {
                //we already had a target. Nothing has changed.
            }
        }
        else
        {
            if (_target_acquired)
            {
                _target_acquired = false;
                if (_target_lost_callback != 0)
                {
                    _target_lost_callback(_pin);
                }
            }
            else
            {
                //we didn't have a target anyways. Nothing has changed.
            }
        }
    }
}

bool IRSensor::isTargetAcquired()
{
    return _target_acquired;
}
