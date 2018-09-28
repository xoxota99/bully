#ifndef I_STRATEGY_H
#define I_STRATEGY_H
#include <Arduino.h>
#include "Motor.h"
#include "IRSensor.h"
#include "LineSensor.h"

class Strategy
{
  public:
    Strategy(Motor motor_left, Motor motor_right, IRSensor sensor_front, IRSensor sensor_left, IRSensor sensor_right, LineSensor ls_left, LineSensor ls_right) {}
    virtual ~Strategy() {}
    virtual void method1() = 0;
    virtual void method2() = 0;
}

#endif //I_STRATEGY_H