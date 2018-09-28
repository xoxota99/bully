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
    const int MAX_VALUE = 1023, MIN_VALUE = 0;
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
    LineSensor(int pin, sense_callback_t callback)
        : LineSensor(pin, 4, DISCRETE, callback) {}
    LineSensor(int pin, int smoothing, op_mode_t op_mode, sense_callback_t callback)
        : LineSensor(pin, 450, 575, smoothing, op_mode, callback) {}
    LineSensor(int pin, int low_threshold, int high_threshold, int smoothing, op_mode_t op_mode, sense_callback_t callback);
    void loop();
    int getValue();
    op_mode_t getOpMode();
};
#endif // LINE_SENSOR_H
