#include <Arduino.h>
#include "MotorController.h"

void MotorController::forward(int speed, unsigned long millis) {}
void MotorController::back(int speed, unsigned long millis) {}
void MotorController::left(int speed, unsigned long millis) {}
void MotorController::right(int speed, unsigned long millis) {}

//interrupt an in-process movement.
void MotorController::interrupt() {}
