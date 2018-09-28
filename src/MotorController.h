#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H
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
    unsigned long commandStartTime = 0;
    MotorController(){};

  public:
    MotorController(Motor left_motor, Motor right_motor, move_completed_t movement_completed_callback);
    void forward(int speed) { forward(speed, -1); }
    void back(int speed) { back(speed, -1); }
    void left(int speed) { left(speed, -1); }
    void right(int speed) { right(speed, -1); }

    void forward(int speed, unsigned long millis);
    void back(int speed, unsigned long millis);
    void left(int speed, unsigned long millis);
    void right(int speed, unsigned long millis);

    void interrupt(); //interrupt an in-process movement.
};

#endif