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

#ifndef __BULLY_CONFIG_H__
#define __BULLY_CONFIG_H__

#ifdef CORE_TEENSY
//Software reset macros / MMap FOR TEENSY ONLY
#define CPU_RESTART_VAL 0x5FA0004                         // write this magic number...
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C           // to this memory location...
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL) // presto!
#endif

#define LED_PIN LED_BUILTIN
#define COUNTDOWN_TIMEOUT_MILLIS 5000          //countdown timeout, in millis.
#define DEBOUNCE_TIMEOUT_MILLIS 50             //timeout to debounce button pushes, in millis.
#define TARGET_PERSISTENCE_TIMEOUT_MILLIS 2000 //timeout after losing sight of a target when we go to "patrol" mode.
#define LOG_LEVEL Logger::DEBUG

// uncomment to enable motors
#define MOTOR_ENABLE

// pin defintions
// Button pins
const int BTN_PIN = 12;                                              // Button pin
const int MOTOR_L_B_PIN = 2, MOTOR_L_PWM_PIN = 3, MOTOR_L_A_PIN = 4; // Left motor pins
const int MOTOR_R_B_PIN = 5, MOTOR_R_PWM_PIN = 6, MOTOR_R_A_PIN = 7; // Right motor pins
const int LEFT_IR_PIN = 18, RIGHT_IR_PIN = 15, FRONT_IR_PIN = 22;    // IR sensor pins.
const int LEFT_FRONT_LINE_PIN = 19, RIGHT_FRONT_LINE_PIN = 16;       // Line Sensing pins.

#endif //__BULLY_CONFIG_H__