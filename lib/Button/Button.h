/**
 * Generic support for a button input on a GPIO pin.
 * Supports ACTIVE_LOW or ACTIVE_HIGH modes, and event callbacks.
 * */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <Arduino.h>
#include <iotypes.h>

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
    Button(io_mode_t ioMode, int pinMode, int pin,
           button_down_t buttonDownCallBack,
           button_up_t buttonUpCallBack);
    Button(int pin, button_down_t buttonDownCallBack,
           button_up_t buttonUpCallBack) : Button(ACTIVE_LOW, INPUT_PULLUP, pin, buttonDownCallBack, buttonUpCallBack) {}
    ~Button(){};
    void loop();
    bool isPressed();
    io_mode_t getIOMode();
};

#endif // __BUTTON_H__
