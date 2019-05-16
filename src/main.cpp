/*
   bully
   Copyright (C) 2018  Philippe Desrosiers

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <Logger.h>
#include <Button.h>
#include <IRSensor.h>
#include <LineSensor.h>
#include <Motor.h>
#include <MotorController.h>

#include "commands.h"
#include "config.h"

enum run_state_t
{
    STARTUP,
    WAITING,
    COUNTDOWN,
    RUNNING,
    STOPPED
};

const char *state_names[] = {
    "STARTUP",
    "WAITING",
    "COUNTDOWN",
    "RUNNING",
    "STOPPED"};

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

void ExitRunState(run_state_t old_state);
void EnterRunState(run_state_t new_state);
void SetRunState(run_state_t new_state);

void OnButtonDown(int pin);
void OnButtonUp(int pin);
void OnTargetAcquired(int pin);
void OnTargetLost(int pin);
void OnLineSense(int pin, int value);
void OnMoveComplete(movement_t move_type, unsigned long duration_millis);

void LoopStateWait();
void LoopStateCountdown();
void LoopStateRunning();
void LoopStateStopped();

typedef void (*run_state_loop_handler_t)();

run_state_loop_handler_t loop_handlers[] = {0,
                                            LoopStateWait,
                                            LoopStateCountdown,
                                            LoopStateRunning,
                                            LoopStateStopped};

run_state_t run_state = STARTUP;

Button button(BTN_PIN, OnButtonDown, OnButtonUp);

Motor motor_left(MOTOR_L_PWM_PIN, MOTOR_L_A_PIN, MOTOR_L_B_PIN);
Motor motor_right(MOTOR_R_PWM_PIN, MOTOR_R_A_PIN, MOTOR_R_B_PIN);
MotorController motor_controller(&motor_left, &motor_right, OnMoveComplete);

IRSensor sensor_front(FRONT_IR_PIN, OnTargetAcquired, OnTargetLost);
IRSensor sensor_left(LEFT_IR_PIN, ACTIVE_LOW, OnTargetAcquired, OnTargetLost);
IRSensor sensor_right(RIGHT_IR_PIN, ACTIVE_LOW, OnTargetAcquired, OnTargetLost);

LineSensor line_sensor_left(LEFT_FRONT_LINE_PIN, 4, DISCRETE, OnLineSense);
LineSensor line_sensor_right(RIGHT_FRONT_LINE_PIN, 4, DISCRETE, OnLineSense);

bool line_status[] = {false, false};

target_location_t last_known_target = NONE;
unsigned long last_target_lock_time = 0;

unsigned long countdown_start_time = 0;

handedness_t handedness;

unsigned long last_button_event_time = 0;

//to account for the fact that we transition to COUNTDOWN on button up, but we
//transition to WAITING on button DOWN. Without this, we would just go immediately
//into countdown again on button_up.
bool ignore_next_button_up = false;

/*
 * Run whenever we transition from one state to another.
 */
void ExitRunState(run_state_t old_state)
{
    int stateCount = sizeof(state_names) / sizeof(state_names[0]);
    if (old_state < stateCount)
    {
        Logger::debug("Now leaving state: '%s'", state_names[old_state]);
    }

    switch (old_state)
    {
    case RUNNING:
        motor_controller.stop();
        break;
    case STARTUP:
    case WAITING:
    case COUNTDOWN:
    case STOPPED:
        break;
    default:
        Logger::warn("Exiting unknown run state (%d)\n", run_state);
    }
}

void EnterRunState(run_state_t new_state)
{
    int stateCount = sizeof(state_names) / sizeof(state_names[0]);
    if (new_state < stateCount)
    {
        Logger::debug("Now entering state: '%s'", state_names[new_state]);
    }

    run_state = new_state;
    switch (run_state)
    {
    case WAITING:
        flushCommandInput();
        break;
    case COUNTDOWN:
        countdown_start_time = millis();
        break;
    case RUNNING:
        Logger::debug("FIGHT!!!");
        // TODO: Right about here, we want to create some "start fight" behavior.
        // We know we're starting up at 90 degrees to the enemy (roughly). Do we want to dodge?
        break;
    case STARTUP:
    case STOPPED:
        break;
    default:
        Logger::warn("Entering unknown run state (%d)\n", new_state);
    }
}

void SetRunState(run_state_t new_state)
{
    ExitRunState(run_state);
    EnterRunState(new_state);
}

void OnMoveComplete(movement_t move_type, unsigned long duration_millis)
{
    Logger::debug("Finished moving.");
}

void OnButtonDown(int pin)
{

    if (millis() - last_button_event_time > DEBOUNCE_TIMEOUT_MILLIS)
    { // debounce
        Logger::debug("button_down");
        last_button_event_time = millis();
        switch (run_state)
        {
        case COUNTDOWN:
        case RUNNING:
            SetRunState(WAITING);
            ignore_next_button_up = true;
        default:
            break;
        }
    }
}

void OnButtonUp(int pin)
{
    if (millis() - last_button_event_time > DEBOUNCE_TIMEOUT_MILLIS)
    { // debounce
        Logger::debug("button_up");
        last_button_event_time = millis();
        if (!ignore_next_button_up)
        {
            switch (run_state)
            {
            case WAITING:
                SetRunState(COUNTDOWN);
            default:
                break;
            }
        }
        ignore_next_button_up = false;
    }
}

/*
    Move in some way (Forward, backward,or turning left/right) until a line sensor or IR sensor hits.
*/
void LoopStateRunning()
{
    // Within the RUNNING state, we sort of have three implied sub-states:
    //  - PATROL (looking for enemy)
    //  - ATTACK (found enemy)
    //  - RETREAT (back away from the edge of the ring).

    // Check sensors
    if (line_status[0]) //left line sensor hit.
    {
        if (line_status[1]) //right line sensor hit.
        {
            //back up straight.
            motor_controller.back(255, 1000);
        }
        else
        {
            //back up curving slightly right.
            motor_controller.move(-255, -128, 500);
        }
    }
    else if (line_status[1]) //right line sensor hit.
    {
        //back up curving slightly left.
        motor_controller.move(-128, -255, 500);
    }
    else
    {
        //no line detected.
        if (last_known_target != NONE && millis() - last_target_lock_time <= TARGET_PERSISTENCE_TIMEOUT_MILLIS)
        {
            switch (last_known_target)
            {
            case LEFT:
                //turn left until something interesting happens.
                motor_controller.left(255);
                break;
            case RIGHT:
                //turn right until something interesting happens.
                motor_controller.right(255);
                break;
            case FRONT:
                //full speed ahead until something interesting happens.
                motor_controller.forward(255);
                break;
            default:
                Logger::warn("last_known_target is unrecognized: %d", last_known_target);
            }
        }
        else
        {
            // we never had a target, or it's been a good long while since we've seen a target.
            // spin in a direction, looking for a target.
            switch (handedness)
            {
            case LEFTHAND:
                motor_controller.left(128);
                break;
            case RIGHTHAND:
            default:
                motor_controller.right(128);
            }
        }
    }
}

void LoopStateStopped()
{
    // someone hit the emergency brake. do some cleanup, then transition to WAITING.
    digitalWriteFast(LED_PIN, LOW);

    // do some cleanup, then transition to WAITING;
    // We do this here in the loop because the next best place to do it would be in the
    // EnterRunState() function, which would result in a recursive call to EnterRunState(),
    // which gives me the heebie-jeebies. The end result is that this loop shouldn't really ever
    // be run more than once before transitioning state.
    SetRunState(WAITING);
}

void LoopStateWait()
{
    //waiting for someone to press my button and start the countdown.
    //do nothing. Sensors are attached and looping, though.

    shell_task(); //process any incoming serial commands.
}

void LoopStateCountdown()
{
    unsigned long elapsed = millis() - countdown_start_time;

    if (elapsed % 1000 == 0)
    {
        unsigned long t = 5 - ((int)(elapsed / 1000.0));
        Logger::info("Countdown: %d\n", t);
        digitalWriteFast(LED_PIN, HIGH);
        delay(100);
    }
    else
    {
        digitalWriteFast(LED_PIN, LOW);
    }

    if (elapsed >= COUNTDOWN_TIMEOUT_MILLIS)
    {
        countdown_start_time = 0;
        SetRunState(RUNNING);
    }
}

void OnTargetAcquired(int pin)
{
    last_target_lock_time = millis();
    switch (pin)
    {
    case LEFT_IR_PIN:
        Logger::debug("Target acquired on LEFT PIN %d\n", pin);
        last_known_target = LEFT;
        break;
    case RIGHT_IR_PIN:
        Logger::debug("Target acquired on RIGHT PIN %d\n", pin);
        last_known_target = RIGHT;
        break;
    case FRONT_IR_PIN:
        Logger::debug("Target acquired on FRONT PIN %d\n", pin);
        last_known_target = FRONT;
        break;
    default:
        Logger::debug("Target acquired on UNKNOWN PIN %d\n", pin);
        last_known_target = NONE;
    }
}

void OnTargetLost(int pin)
{
    last_target_lock_time = millis();
    switch (pin)
    {
    case LEFT_IR_PIN:
        Logger::debug("Target lost on LEFT PIN %d\n", pin);
        break;
    case RIGHT_IR_PIN:
        Logger::debug("Target lost on RIGHT PIN %d\n", pin);
        break;
    case FRONT_IR_PIN:
        Logger::debug("Target lost on FRONT PIN %d\n", pin);
        break;
    default:
        Logger::debug("Target lost on UNKNOWN PIN %d\n", pin);
    }
}

void OnLineSense(int pin, int senseValue)
{
    const char *dString;
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
        Logger::debug("%s on LEFT FRONT pin %d\n", dString, pin);
        line_status[0] = senseValue == 0;
        break;
    case RIGHT_FRONT_LINE_PIN:
        Logger::debug("%s on RIGHT FRONT pin %d\n", dString, pin);
        line_status[1] = senseValue == 0;
        break;
    default:
        Logger::debug("%s on UNKNOWN PIN %d\n", dString, pin);
        break;
    }
}

//=====
// process shell commands

int cmd_sense(int argc, char **argv)
{
    if (argc != 1)
    {
        Serial.printf("Usage: s\n");
        return SHELL_RET_FAILURE;
    }

    Serial.printf("Sensor Values========\n");
    Serial.printf("Left IR: %s\t(pin %d = %s)\n", (sensor_left.isTargetAcquired() ? "Acquired" : "None"), LEFT_IR_PIN, (digitalRead(LEFT_IR_PIN) ? "HIGH" : "LOW"));
    Serial.printf("Right IR: %s\t(pin %d = %s)\n", (sensor_right.isTargetAcquired() ? "Acquired" : "None"), RIGHT_IR_PIN, (digitalRead(RIGHT_IR_PIN) ? "HIGH" : "LOW"));
    Serial.printf("Front IR: %s\t(pin %d = %s)\n", (sensor_front.isTargetAcquired() ? "Acquired" : "None"), FRONT_IR_PIN, (digitalRead(FRONT_IR_PIN) ? "HIGH" : "LOW"));
    Serial.printf("\n");
    Serial.printf("Left LineSense: %s\t(pin %d = %d)\n", (line_sensor_left.getDigitalValue() ? "No Line" : "Line"), LEFT_FRONT_LINE_PIN, analogRead(LEFT_FRONT_LINE_PIN));
    Serial.printf("Right LineSense: %s\t(pin %d = %d)\n", (line_sensor_right.getDigitalValue() ? "No Line" : "Line"), RIGHT_FRONT_LINE_PIN, analogRead(RIGHT_FRONT_LINE_PIN));
    Serial.printf("\n");
    Serial.printf("Left Motor: %s %d\n", (motor_left.getSpeed() < 0 ? "BACKWARD" : (motor_left.getSpeed() == 0 ? "STOPPED" : "FORWARD")), abs(motor_left.getSpeed()));
    Serial.printf("Right Motor: %s %d\n", (motor_right.getSpeed() < 0 ? "BACKWARD" : (motor_right.getSpeed() == 0 ? "STOPPED" : "FORWARD")), abs(motor_right.getSpeed()));

    return SHELL_RET_SUCCESS;
}

int cmd_calibrate(int argc, char **argv)
{
    if (argc != 2)
    {
        Serial.printf("Usage: c [il|if|ir|ll|lr]\n");
        return SHELL_RET_FAILURE;
    }

    //Do something.
    Serial.printf("Command 'c' is not yet supported.\n");

    return SHELL_RET_SUCCESS;
}

int cmd_move(int argc, char **argv)
{
    if (argc == 1)
    {
        Serial.printf("Usage: mv <speed1> <speed2> <duration>\n\n\t- speed1 - Speed of Motor1 (from -255 to 255)\n\t- speed2 - Speed of Motor2 (from -255 to 255)\n\t- duration - duration (in milliseconds) to move for.\n");
        return SHELL_RET_FAILURE;
    }
    Serial.printf("Command 'mv' is not yet supported.\n");
    //Do something.

    return SHELL_RET_SUCCESS;
}

void setup()
{
    Logger::level = LOG_LEVEL;

    Serial.begin(9600);
    delay(10000);

    pinMode(LED_PIN, OUTPUT);
    digitalWriteFast(LED_PIN, HIGH);

    handedness = random(0, 2) == 0 ? LEFTHAND : RIGHTHAND; //decides which way the robot will spin when "pinging".

    shell_init(shell_reader, shell_writer, 0);
    const int c1 = sizeof(commands);
    if (c1 > 0)
    {
        const int c2 = sizeof(commands[0]);
        const int ccount = c1 / c2;

        for (int i = 0; i < ccount; i++)
        {
            Logger::debug("Registering command: %s", commands[i].shell_command_string);
            if (!shell_register(commands[i].shell_program, commands[i].shell_command_string))
            {
                Logger::error("could not register shell command '%s'.", commands[i].shell_command_string);
            }
        }
    }

    SetRunState(WAITING); //done with startup, waiting for the button press.
}

void loop()
{
    button.loop();

#ifdef MOTOR_ENABLE

    motor_controller.loop();

#endif //MOTOR_ENABLE

    sensor_front.loop();
    sensor_left.loop();
    sensor_right.loop();

    // line sensors are sensitive to reflectance. So a reflective dark surface
    // looks a lot like a reflective light surface. We need some sort of calibration / normalization routine.
    line_sensor_left.loop();
    line_sensor_right.loop();

    if (loop_handlers[run_state])
        loop_handlers[run_state]();
}
