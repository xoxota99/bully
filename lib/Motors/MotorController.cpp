#include <Arduino.h>
#include "MotorController.h"

MotorController::MotorController(Motor *left_motor, Motor *right_motor, move_completed_t movement_completed_callback)
{
    _left_motor = left_motor;
    _right_motor = right_motor;
    _movement_completed_callback = movement_completed_callback;
}

void MotorController::move(int leftSpeed, int rightSpeed, unsigned long duration_millis)
{
    _command_duration = duration_millis;
    _command_start_time = millis();
    _left_motor->move(leftSpeed);
    _right_motor->move(rightSpeed);
}

//interrupt an in-process movement.
void MotorController::stop()
{
    _command_duration = millis() - _command_start_time - 1;
    move(0, 0);
}

void MotorController::loop()
{
    if (_command_duration < 0 || millis() - _command_start_time >= _command_duration)
    {
        //free wheel.
        move(0, 0);
    }
}
