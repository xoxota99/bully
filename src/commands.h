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

#ifndef __BULLY_COMMANDS_H__
#define __BULLY_COMMANDS_H__

#include <Shell.h> //https://github.com/geekfactory/Shell
#include "config.h"

typedef struct Command
{
    shell_program_t shell_program;
    const char *shell_command_string;
    const char *shell_help_string;
} Command;

void processCommands();
void flushCommandInput();
int shell_reader(char *data);
void shell_writer(char data);

int cmd_help(int argc, char **argv);
int cmd_sense(int argc, char **argv);
int cmd_calibrate(int argc, char **argv);
int cmd_log(int argc, char **argv);
#ifdef CPU_RESTART
int cmd_reset(int argc, char **argv);
#endif
#ifdef MOTOR_ENABLE
int cmd_move(int argc, char **argv);
#endif
//===

const Command commands[] = {
    {cmd_help, "help", "This message."},
    {cmd_sense, "s", "Display Sensor inputs."},
    {cmd_calibrate, "c", "Calibrate sensors."},
    {cmd_log, "log", "Change Log Level."},

#ifdef CPU_RESTART
    {cmd_reset, "reset", "Reset System."},
#endif

#ifdef MOTOR_ENABLE
    {cmd_move, "mv", "spin motors"},
#endif

    {cmd_help, "?", "Synonym for 'help'"}};

#endif //__BULLY_COMMANDS_H__
