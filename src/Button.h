/**
 * Generic support for a button input on a GPIO pin.
 * Supports ACTIVE_LOW or ACTIVE_HIGH modes, and event callbacks.
 * */

#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "types.h"

typedef void (*button_down_t)(int);
typedef void (*button_up_t)(int);

class Button
{
  private:
    int _input_pin = 0;
    int _pin_mode = INPUT_PULLUP;
    io_mode_t _io_mode = ACTIVE_LOW;
    button_down_t _button_down_callback = 0;
    button_up_t _button_up_callback = 0;

    bool _pressed = false;
    bool _attached = true;

    Button(){};

  public:
    static Button attach(io_mode_t ioMode, int pinMode, int pin,
                         button_down_t buttonDownCallBack,
                         button_up_t buttonUpCallBack);
    static Button attach(int pin, button_down_t buttonDownCallBack,
                         button_up_t buttonUpCallBack);
    void detach();
    void loop();
    bool isPressed();
    io_mode_t getIOMode();
};

#endif // BUTTON_H
