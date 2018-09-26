/**
 * Generic support for NPN-style IR reflectance sensors.
 * Supports ACTIVE_LOW or ACTIVE_HIGH modes, callbacks on
 * rising / falling edge.
 * */

#ifndef IR_SENSOR_H
#define IR_SENSOR_H
#include "types.h"
#include <Arduino.h>

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
    bool _attached = true;

    IRSensor(){};

  public:
    static IRSensor attach(int pin, io_mode_t ioMode,
                           target_acquire_callback_t target_acquired_callback,
                           target_lost_callback_t target_lost_callback);
    static IRSensor attach(int pin,
                           target_acquire_callback_t target_acquired_callback,
                           target_lost_callback_t target_lost_callback);
    void detach();
    void loop();
    bool isTargetAcquired();
};

#endif // IR_SENSOR_H
