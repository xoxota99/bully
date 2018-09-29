#include "Motor.h"

Motor::Motor(int pwm_pin, int pin_a, int pin_b)
{
    Motor m;
    m._pwm_pin = pwm_pin;
    m._pin_a = pin_a;
    m._pin_b = pin_b;

    pinMode(pwm_pin, OUTPUT);
    pinMode(pin_a, OUTPUT);
    pinMode(pin_b, OUTPUT);

    m.move(0);
}

Motor::~Motor()
{
    digitalWriteFast(_pwm_pin, LOW);
    digitalWriteFast(_pin_a, LOW);
    digitalWriteFast(_pin_b, LOW);
}

/*
    speed = a number between -255 and 255. The *Actual* movement occurs in the loop() function.
*/
void Motor::move(int speed)
{
    _motor_speed = min(255, max(-255, speed));
}

int Motor::getSpeed()
{
    return _motor_speed;
}

void Motor::loop()
{
    if (_attached)
    {
        digitalWriteFast(_pin_a, (_motor_speed > 0 ? HIGH : LOW));
        digitalWriteFast(_pin_b, (_motor_speed < 0 ? HIGH : LOW));

        analogWrite(_pwm_pin, abs(_motor_speed)); // 0 to 255
    }
}
