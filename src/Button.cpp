#include "Button.h"

Button Button::attach(io_mode_t ioMode, int pinMode, int pin,
                      button_down_t buttonDownCallBack,
                      button_up_t buttonUpCallBack)
{
    Button b;
    b._io_mode = ioMode;
    b._pin_mode = pinMode;
    b._input_pin = pin;
    b._button_down_callback = buttonDownCallBack;
    b._button_up_callback = buttonUpCallBack;
    return b;
}

Button Button::attach(int pin, button_down_t buttonDownCallBack,
                      button_up_t buttonUpCallBack)
{
    Button b;
    b._input_pin = pin;
    b._button_down_callback = buttonDownCallBack;
    b._button_up_callback = buttonUpCallBack;
    return b;
}

void Button::detach() { _attached = false; }

io_mode_t Button::getIOMode() { return _io_mode; }

void Button::loop()
{
    if (_attached)
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
                // nothing has happened.
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
                // nothing has happened.
            }
        }
    }
}

bool Button::isPressed() { return _pressed; }
