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

#include "commands.h"
#include <Logger.h>
#include <Shell.h>
#include <Arduino.h>

void flushCommandInput()
{
    while (Serial.available())
    {
        Serial.read();
    }
}

/**
   Function to read data from serial port
   Functions to read from physical media should use this prototype:
   int my_reader_function(char * data)
 */
int shell_reader(char *data)
{
    // Wrapper for Serial.read() method
    if (Serial.available())
    {
        *data = Serial.read();
        return 1;
    }
    return 0;
}

/**
   Function to write data to serial port
   Functions to write to physical media should use this prototype
   void my_writer_function(char data)
 */
void shell_writer(char data)
{
    Serial.write(data);
}

#ifdef CPU_RESTART
int cmd_reset(int argc, char **argv)
{
    CPU_RESTART;
    return SHELL_RET_SUCCESS; //unreachable?
}
#endif

int cmd_log(int argc, char **argv)
{
    if (argc == 1)
    {
        Serial.printf("%s\n", Logger::getLevelName());
    }

    if (argc != 2)
    {
        Serial.printf("Usage: log [TRACE | DEBUG | INFO | WARN | ERROR | FATAL]\n");
        return SHELL_RET_FAILURE;
    }

    char *token = argv[1];

    if (strncmp(token, "TRACE", 5) == 0)
    {
        Logger::level = Logger::TRACE;
    }
    else if (strncmp(token, "DEBUG", 5) == 0)
    {
        Logger::level = Logger::DEBUG;
    }
    else if (strncmp(token, "INFO", 4) == 0)
    {
        Logger::level = Logger::INFO;
    }
    else if (strncmp(token, "WARN", 4) == 0)
    {
        Logger::level = Logger::WARN;
    }
    else if (strncmp(token, "ERROR", 5) == 0)
    {
        Logger::level = Logger::ERROR;
    }
    else if (strncmp(token, "FATAL", 5) == 0)
    {
        Logger::level = Logger::FATAL;
    }
    else
    {
        Serial.printf("Usage: log [TRACE | DEBUG | INFO | WARN | ERROR | FATAL]\n");
        return SHELL_RET_FAILURE;
    }

    return SHELL_RET_SUCCESS;
}

/**
   Print some help.
 */
int cmd_help(int argc, char **argv)
{
    shell_print_commands();
    return SHELL_RET_SUCCESS;
    /*
    int size = *(&commands + 1) - commands;
    for (int i = 0; i < size; i++)
    {
        Serial.printf("%s\t\t%s\n", commands[i].shell_command_string, commands[i].shell_help_string);
    }
    return SHELL_RET_SUCCESS;
    */
}
