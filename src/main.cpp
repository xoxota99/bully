#include "Button.h"
#include "IRSensor.h"
#include "LineSensor.h"
#include "Motor.h"
#include <Arduino.h>

#define LED_PIN LED_BUILTIN
#define COUNTDOWN_TIMEOUT 5000000 //countdown timeout, in micros.

// pin defintions
// Button pins
const int BTN_PIN = 12;                                              // Button pins
const int MOTOR_L_B_PIN = 2, MOTOR_L_PWM_PIN = 3, MOTOR_L_A_PIN = 4; // Left motor pins
const int MOTOR_R_B_PIN = 5, MOTOR_R_PWM_PIN = 6, MOTOR_R_A_PIN = 7; // Right motor pins
const int LEFT_IR_PIN = 18, RIGHT_IR_PIN = 15, FRONT_IR_PIN = 22;    // IR sensor pins.
const int LEFT_FRONT_LINE_PIN = 19, RIGHT_FRONT_LINE_PIN = 16;       // Line Sensing pins.

enum runState_t
{
    STARTUP,
    WAITING,
    COUNTDOWN,
    RUNNING,
    STOPPED
};

enum target_location_t
{
    LEFT,
    RIGHT,
    FRONT,
    NONE
};

enum handedness_t
{
    LEFTHAND,
    RIGHTHAND
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

run_state_loop_handler_t loop_handlers[] = {loop_STARTUP,
                                            loop_WAITING,
                                            loop_COUNTDOWN,
                                            loop_RUNNING,
                                            loop_STOPPED};

runState_t runState = STARTUP;

Button button = Button::attach(BTN_PIN, buttonDown_callback, buttonUp_callback);

Motor motorLeft = Motor::attach(MOTOR_L_PWM_PIN, MOTOR_L_A_PIN, MOTOR_L_B_PIN);
Motor motorRight = Motor::attach(MOTOR_R_PWM_PIN, MOTOR_R_A_PIN, MOTOR_R_B_PIN);

IRSensor frontSensor = IRSensor::attach(FRONT_IR_PIN, targetAcquired_callback, targetLost_callback);
IRSensor leftSensor = IRSensor::attach(LEFT_IR_PIN, targetAcquired_callback, targetLost_callback);
IRSensor rightSensor = IRSensor::attach(RIGHT_IR_PIN, targetAcquired_callback, targetLost_callback);

LineSensor leftLineSensor = LineSensor::attach(LEFT_FRONT_LINE_PIN, 4, DISCRETE, lineSensor_callback);
LineSensor rightLineSensor = LineSensor::attach(RIGHT_FRONT_LINE_PIN, 4, DISCRETE, lineSensor_callback);

bool leftFrontLineStatus = false, rightFrontLineStatus = false;

target_location_t lastKnownTarget = NONE;
unsigned long lastTargetLockTime = 0;
unsigned long targetPersistenceTimeout = 2000000;

unsigned long countDownStartTime = 0;

handedness_t handedness;

/*
 * Run whenever we transition from one state to another.
 */
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

void enterState(runState_t newState)
{
    runState = newState;
    switch (runState)
    {
    case STARTUP:
        break;
    case WAITING:
        break;
    case COUNTDOWN:
        countDownStartTime = micros();
        break;
    case RUNNING:
        //TODO: Right about here, we want to create some "start fight" behavior. We know we're starting up at 90 degrees to the enemy (roughly). Do we want to dodge?
        break;
    case STOPPED:
        break;
    default:
        Serial.printf("Entering unknown run state (%d)\n", newState);
    }
}

void setRunState(runState_t newState)
{
    Serial.printf("setRunState: %d\n", newState);
    exitState(runState);
    enterState(newState);
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

void buttonUp_callback(int pin)
{
}

void loop_STARTUP()
{
    // do nothing.
}

/*
    Move in some way (Forward, backward,or turning left/right) until a line sensor or IR sensor hits.
*/
void loop_RUNNING()
{
    // Check sensors
    if (leftFrontLineStatus)
    {
        if (rightFrontLineStatus)
        {
            //back up straight.
            motorRight.move(-255);
            motorLeft.move(-255);
        }
        else
        {
            //back up curving slightly right.
            motorRight.move(-128);
            motorLeft.move(-255);
        }
    }
    else if (rightFrontLineStatus)
    {
        //back up curving slightly left.
        motorRight.move(-255);
        motorLeft.move(-128);
    }
    else
    {
        //no line detected.
        if (lastKnownTarget != NONE && lastTargetLockTime <= targetPersistenceTimeout)
        {
            switch (lastKnownTarget)
            {
            case LEFT:
                //turn left until something interesting happens.
                motorRight.move(255);
                motorLeft.move(-255);
            case RIGHT:
                //turn right until something interesting happens.
                motorRight.move(-255);
                motorLeft.move(255);
            case FRONT:
                //full speed ahead until something interesting happens.
                motorRight.move(255);
                motorLeft.move(255);
            default:
            }
        }
        else
        {
            // we never had a target, or it's been a good long while since we've seen a target.
            // spin in a direction.
            switch (handedness)
            {
            case LEFTHAND:
                motorRight.move(255);
                motorLeft.move(-255);
            case RIGHTHAND:
            default:
                motorRight.move(-255);
                motorLeft.move(255);
            }
        }
    }
}

void loop_STOPPED()
{
    // someone hit the emergency brake. do some cleanup, then transition to WAITING.
    digitalWriteFast(LED_BUILTIN, LOW);

    // do some cleanup, then transition to WAITING;
    // We do this here in the loop because the next best place to do it would be in the
    // enterState() function, which would result in a recursive call to enterState(),
    // which gives me the heebie-jeebies. The end result is that this loop shouldn't really ever
    // be run more than once before transitioning state.
    setRunState(WAITING);
}

void loop_WAITING()
{
    //waiting for someone to press my button and start the countdown.
    //do nothing. Sensors are attached and looping, though.
}

void loop_COUNTDOWN()
{
    unsigned long elapsed = micros() - countDownStartTime;

    if (elapsed >= COUNTDOWN_TIMEOUT)
    {
        countDownStartTime = 0;
        setRunState(RUNNING);
    }

    if (elapsed % 1000000 == 0)
    {
        // unsigned long t = (int)(elapsed / 1000000.0);
        Serial.printf("%d micros\n", elapsed);
        digitalWriteFast(LED_BUILTIN, HIGH);
        delay(100);
    }
    else
    {
        digitalWriteFast(LED_BUILTIN, LOW);
    }
}

void targetAcquired_callback(int pin)
{
    lastTargetLockTime = micros();
    switch (pin)
    {
    case LEFT_IR_PIN:
        Serial.printf("Target acquired on LEFT PIN %d\n", pin);
        lastKnownTarget = LEFT;
        break;
    case RIGHT_IR_PIN:
        Serial.printf("Target acquired on RIGHT PIN %d\n", pin);
        lastKnownTarget = RIGHT;
        break;
    case FRONT_IR_PIN:
        Serial.printf("Target acquired on FRONT PIN %d\n", pin);
        lastKnownTarget = FRONT;
        break;
    default:
        Serial.printf("Target acquired on UNKNOWN PIN %d\n", pin);
        lastKnownTarget = NONE;
        break;
    }
}

void targetLost_callback(int pin)
{
    lastTargetLockTime = micros();
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
    char *dString;
    if (senseValue == 0)
    {
        //line
        dString = "Line detected";
    }
    else
    {
        //no line.
        dString = "Line not detected";
    }

    switch (pin)
    {
    case LEFT_FRONT_LINE_PIN:
        Serial.printf("%s on LEFT FRONT pin %d\n", dString, pin);
        leftFrontLineStatus = senseValue == 0;
        break;
    case RIGHT_FRONT_LINE_PIN:
        Serial.printf("%s on RIGHT FRONT pin %d\n", dString, pin);
        rightFrontLineStatus = senseValue == 0;
        break;
    default:
        Serial.printf("%s on UNKNOWN PIN %d\n", dString, pin);
        break;
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWriteFast(LED_BUILTIN, LOW);

    handedness = random(0, 2) == 0 ? LEFTHAND : RIGHTHAND; //decides which way the robot will spin when "pinging".

    runState = WAITING; //done with startup, waiting for the button press.
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
