#include "Button.h"
#include "IRSensor.h"
#include "LineSensor.h"
#include "Motor.h"
#include <Arduino.h>

#define LED_PIN LED_BUILTIN

// Button pins
const int BTN_GND_PIN = 10, BTN_IN_PIN = 12;

// Left motor pins
const int MOTOR_L_B_PIN = 2, MOTOR_L_PWM_PIN = 3, MOTOR_L_A_PIN = 4;

// Right motor pins
const int MOTOR_R_B_PIN = 5, MOTOR_R_PWM_PIN = 6, MOTOR_R_A_PIN = 7;

// IR sensor pins.
const int LEFT_IR_PIN = 18, RIGHT_IR_PIN = 15, FRONT_IR_PIN = 22;

// Line Sensing pins.
const int LEFT_FRONT_LINE_PIN = 19, RIGHT_FRONT_LINE_PIN = 16;

enum runState_t
{
    STARTUP,
    WAITING,
    COUNTDOWN,
    RUNNING,
    STOPPED
};

void exitState(runState_t oldState);
void setRunState(runState_t newState);

void buttonDown_callback(int pin);
void buttonUp_callback(int pin);
void targetAcquired_callback(int pin);
void targetLost_callback(int pin);
void lineSensor_callback(int pin, int value);

void loop_STARTUP();
void loop_WAITING();
void loop_COUNTDOWN();
void loop_RUNNING();
void loop_STOPPED();

typedef void (*run_state_loop_handler_t)();

run_state_loop_handler_t loop_handlers[] = {
    loop_STARTUP, loop_WAITING, loop_COUNTDOWN, loop_RUNNING, loop_STOPPED};

runState_t runState = STARTUP;

Button button =
    Button::attach(BTN_IN_PIN, buttonDown_callback, buttonUp_callback);

Motor motorLeft = Motor::attach(MOTOR_L_PWM_PIN, MOTOR_L_A_PIN, MOTOR_L_B_PIN);
Motor motorRight = Motor::attach(MOTOR_R_PWM_PIN, MOTOR_R_A_PIN, MOTOR_R_B_PIN);

IRSensor frontSensor = IRSensor::attach(FRONT_IR_PIN, targetAcquired_callback,
                                        targetLost_callback);
IRSensor leftSensor =
    IRSensor::attach(LEFT_IR_PIN, targetAcquired_callback, targetLost_callback);
IRSensor rightSensor = IRSensor::attach(RIGHT_IR_PIN, targetAcquired_callback,
                                        targetLost_callback);

LineSensor leftLineSensor =
    LineSensor::attach(LEFT_FRONT_LINE_PIN, 4, DISCRETE, lineSensor_callback);
LineSensor rightLineSensor =
    LineSensor::attach(RIGHT_FRONT_LINE_PIN, 4, DISCRETE, lineSensor_callback);

void exitState(runState_t oldState)
{
    switch (oldState)
    {
    case STARTUP:
        break;
    case WAITING:
        break;
    case COUNTDOWN:
        break;
    case RUNNING:
        break;
    case STOPPED:
        break;
    default:
        Serial.printf("Exiting unknown run state (%d)\n", runState);
    }
}

void setRunState(runState_t newState)
{
    exitState(runState);
    Serial.printf("setRunState: %d\n", newState);
    runState = newState;
}

void buttonDown_callback(int pin)
{
    static unsigned long lastButtonPush = 0;

    if (millis() - lastButtonPush > 100)
    { // debounce
        lastButtonPush = millis();
        switch (runState)
        {
        case STARTUP:
            // do nothing
            break;
        case WAITING:
            setRunState(COUNTDOWN);
            break;
        case COUNTDOWN:
        case RUNNING:
            setRunState(WAITING);
            break;
        default:
            Serial.printf("Button pushed at unknown run state (%d)\n", runState);
            break;
        }
    }
}

void buttonUp_callback(int pin) {}

void loop_STARTUP()
{
    // do nothing.
}

void loop_RUNNING()
{
    // do nothing.
}

void loop_STOPPED()
{
    digitalWriteFast(LED_BUILTIN, LOW);
    // do nothing.
}

void loop_WAITING() {}

void loop_COUNTDOWN()
{
    static unsigned long startTime = millis();
    static bool printed = 0;
    // if (startTime == 0)
    // {
    //     startTime = millis();
    // }

    unsigned long elapsed = millis() - startTime;

    if (elapsed >= 4000)
    {
        startTime = 0;
        printed = 0;
        setRunState(RUNNING);
    }

    if (elapsed % 1000 == 0 && printed == 0)
    {
        unsigned long t = (int)(elapsed / 1000.0);
        Serial.printf("%d seconds\n", 5 - t);
        printed = 1;
        digitalWriteFast(LED_BUILTIN, HIGH);
        delay(100);
    }
    else if (elapsed % 1000 > 0 && printed == 1)
    {
        printed = 0;
        digitalWriteFast(LED_BUILTIN, LOW);
    }
}

void targetAcquired_callback(int pin)
{
    switch (pin)
    {
    case LEFT_IR_PIN:
        Serial.printf("Target acquired on LEFT PIN %d\n", pin);
        break;
    case RIGHT_IR_PIN:
        Serial.printf("Target acquired on RIGHT PIN %d\n", pin);
        break;
    case FRONT_IR_PIN:
        Serial.printf("Target acquired on FRONT PIN %d\n", pin);
        break;
    default:
        Serial.printf("Target acquired on UNKNOWN PIN %d\n", pin);
        break;
    }
}

void targetLost_callback(int pin)
{
    switch (pin)
    {
    case LEFT_IR_PIN:
        Serial.printf("Target lost on LEFT PIN %d\n", pin);
        break;
    case RIGHT_IR_PIN:
        Serial.printf("Target lost on RIGHT PIN %d\n", pin);
        break;
    case FRONT_IR_PIN:
        Serial.printf("Target lost on FRONT PIN %d\n", pin);
        break;
    default:
        Serial.printf("Target lost on UNKNOWN PIN %d\n", pin);
        break;
    }
}

void lineSensor_callback(int pin, int senseValue)
{
    switch (pin)
    {
    case LEFT_FRONT_LINE_PIN:
        Serial.printf("Line detected on LEFT FRONT pin %d\n", pin);
        break;
    case RIGHT_FRONT_LINE_PIN:
        Serial.printf("Line detected on RIGHT FRONT pin %d\n", pin);
        break;
    default:
        Serial.printf("Line detected on UNKNOWN PIN %d\n", pin);
        break;
    }
}

void setup()
{

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWriteFast(LED_BUILTIN, LOW);

    pinMode(BTN_GND_PIN, OUTPUT);
    digitalWriteFast(BTN_GND_PIN, LOW);

    runState = WAITING;
}

void loop()
{
    button.loop();
    if (runState == RUNNING)
    {
        motorLeft.loop();
        motorRight.loop();
    }

    frontSensor.loop();
    leftSensor.loop();
    rightSensor.loop();

    leftLineSensor.loop();
    rightLineSensor.loop();

    loop_handlers[runState]();
}
