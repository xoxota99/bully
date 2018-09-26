/**
 * Generic support for QTR Analog Line Sensors.
 * Supports callbacks on rising / falling edge.
 * */

#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H
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
    int _pin;
    int _smoothing = 4;
    sense_callback_t _callback;
    int _value;
    int _low_threshold = 450;
    int _high_threshold = 575;
    op_mode_t _op_mode = DISCRETE;
    bool _attached = true;

    LineSensor(){};

  public:
    static LineSensor attach(int pin, sense_callback_t callback);
    static LineSensor attach(int pin, int smoothing, op_mode_t op_mode,
                             sense_callback_t callback);
    static LineSensor attach(int pin, int low_threshold, int high_threshold,
                             int smoothing, op_mode_t op_mode,
                             sense_callback_t callback);
    void detach();
    void loop();
    int getValue();
    op_mode_t getOpMode();
};
#endif // LINE_SENSOR_H
