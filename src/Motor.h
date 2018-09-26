/**
 * Support for Pololu VNH5019 Motor Driver carrier board.
 * */

#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>

enum motorState_t
{
    FORWARD,
    BACKWARD,
    BRAKE,
    FREE
};

class Motor
{
  private:
    Motor(){};

    int _motor_speed = 0;
    motorState_t _motor_state = FREE;
    int _pwm_pin, _pin_a, _pin_b;
    bool _attached = true;

  public:
    static Motor attach(int pwm_pin, int pin_a, int pin_b);
    void detach();
    // void forward(uint8_t speed);
    // void backward(uint8_t speed);
    // void brake();
    motorState_t getState();
    void setState(motorState_t state);

    int getSpeed();
    void setSpeed(int speed);

    void loop();
};

#endif // MOTOR_H
