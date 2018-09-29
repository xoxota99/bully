/**
 * Support for Pololu VNH5019 Motor Driver carrier board.
 * */

#ifndef __MOTOR_H__
#define __MOTOR_H__
#include <Arduino.h>

class Motor
{
  private:
    Motor(){};

    int _motor_speed = 0; //between -255 and 255
    int _pwm_pin, _pin_a, _pin_b;
    bool _attached = true;

  public:
    Motor(int pwm_pin, int pin_a, int pin_b);
    ~Motor();
    void move(int speed); //between -255 and 255
    int getSpeed();

    void loop();
};

#endif // __MOTOR_H
