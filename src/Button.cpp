#include "Button.h"

Button::Button(io_mode_t ioMode, int pin_mode, int pin,
               button_down_t buttonDownCallBack,
               button_up_t buttonUpCallBack)
{
    _input_pin = pin;
    _button_down_callback = buttonDownCallBack;
    _button_up_callback = buttonUpCallBack;
    _io_mode = ioMode;
    _pin_mode = pin_mode;
    digitalWriteFast(pin, (ioMode == ACTIVE_LOW ? HIGH : LOW));
    pinMode(pin, pin_mode);
}

io_mode_t Button::getIOMode() { return _io_mode; }

void Button::loop()
{

    int val = digitalReadFast(_input_pin);
    if ((val == HIGH && _io_mode == ACTIVE_HIGH) ||
        (val == LOW && _io_mode == ACTIVE_LOW))
    {
        if (!_pressed)
        {
            // edge detect.
            _pressed = true;
            if (_button_down_callback != 0)
            {
                _button_down_callback(_input_pin);
            }
        }
        else
        {
            // Button was already pressed, and it's still pressed.
        }
    }
    else
    {
        if (_pressed)
        {
            // edge detect.
            _pressed = false;
            if (_button_up_callback != 0)
            {
                _button_up_callback(_input_pin);
            }
        }
        else
        {
            // Button was not pressed, and is still not pressed. Super exciting.
        }
    }
}

bool Button::isPressed() { return _pressed; }
