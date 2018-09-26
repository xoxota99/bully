#include "Motor.h"

Motor Motor::attach(int pwm_pin, int pin_a, int pin_b)
{
    Motor m;
    m._pwm_pin = pwm_pin;
    m._pin_a = pin_a;
    m._pin_b = pin_b;

    m.setSpeed(0);
    m.setState(FORWARD);

    return m;
}

void Motor::detach()
{
    digitalWriteFast(_pwm_pin, LOW);
    digitalWriteFast(_pin_a, LOW);
    digitalWriteFast(_pin_b, LOW);
    _motor_state = FREE;
    _attached = false;
}

motorState_t Motor::getState()
{
    return _motor_state;
}

void Motor::setState(motorState_t state)
{
    _motor_state = state;
}

int Motor::getSpeed()
{
    return _motor_speed;
}

void Motor::setSpeed(int speed)
{
    _motor_speed = speed;
}

void Motor::loop()
{
    if (_attached)
    {
        digitalWriteFast(_pin_a, ((_motor_state == FORWARD || _motor_state == BRAKE) ? HIGH : LOW));
        digitalWriteFast(_pin_b, ((_motor_state == BACKWARD || _motor_state == BRAKE) ? HIGH : LOW));

        analogWrite(_pwm_pin, min(255, max(0, _motor_speed))); // 0 - 255
    }
}
