#ifndef __MOTOR_CONTROLLER_H__
#define __MOTOR_CONTROLLER_H__

#include <Arduino.h>
#include "Motor.h"

enum movement_t
{
    FORWARD,
    BACKWARD,
    LEFTTURN,
    RIGHTTURN
};

typedef void (*move_completed_t)(movement_t, unsigned long);

class MotorController
{
  private:
    unsigned long _command_start_time = 0;
    unsigned long _command_duration = 0;
    Motor *_left_motor, *_right_motor;
    MotorController(){};
    move_completed_t _movement_completed_callback;

  public:
    MotorController(Motor *left_motor, Motor *right_motor, move_completed_t movement_completed_callback);
    void forward(uint8_t speed) { forward(speed, -1); }
    void back(uint8_t speed) { back(speed, -1); }
    void left(uint8_t speed) { left(speed, -1); }
    void right(uint8_t speed) { right(speed, -1); }
    void move(int leftSpeed, int rightSpeed) { move(leftSpeed, rightSpeed, -1); }

    void forward(uint8_t speed, unsigned long duration_millis) { move(speed, speed, duration_millis); }
    void back(uint8_t speed, unsigned long duration_millis) { move(-speed, -speed, duration_millis); }
    void left(uint8_t speed, unsigned long duration_millis) { move(-speed, speed, duration_millis); }
    void right(uint8_t speed, unsigned long duration_millis) { move(speed, -speed, duration_millis); }

    void move(int leftSpeed, int rightSpeed, unsigned long duration_millis);

    void stop(); //interrupt an in-process movement.
    void loop();
};

#endif //  __MOTOR_CONTROLLER_H__