/*
 * clidemo, a example and test bench for my command line library libcli.
 *
 * Copyright (C) 2020 Julian Friedrich
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 *
 * You can file issues at https://github.com/fjulian79/clidemo/issues
 */

#include <Arduino.h>
#include <stm32yyxx_ll_adc.h>

#include <cli/cli.h>
#include <lipo/lipo.hpp>
#include <generic/generic.hpp>

#include <stdio.h>
#include <stdint.h>

#include "param.hpp"

#define CALX_TEMP 25
#define V25       1430
#define AVG_SLOPE 4300

#define VERSIONSTRING      "rel_1_0_0"

typedef struct
{
    BatteryParams_t Battery;

}Parameter_t;

Param<Parameter_t> param;
LiPo lipo(&param.data.Battery);
Cli cli;

int8_t cmd_ver(char *argv[], uint8_t argc)
{
    Serial.printf("\nmotorctrl %s Copyright (C) 2020 Julian Friedrich\n", 
            VERSIONSTRING);
    Serial.printf("build: %s, %s\n", __DATE__, __TIME__);
    Serial.printf("\n");
    Serial.printf("This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you\n");
    Serial.printf("are welcome to redistribute it under certain conditions.\n");
    Serial.printf("See GPL v3 licence at https://www.gnu.org/licenses/ for details.\n");
    Serial.printf("\nThe app is hosted at https://github.com/fjulian79/libcli.git\n\n");

    return 0;
}

int8_t cmd_help(char *argv[], uint8_t argc)
{
    Serial.printf("Supported commands:\n");
    Serial.printf("  set            TBD\n");
    Serial.printf("  rpm            TBD\n");
    Serial.printf("  bat            TBD\n");
    Serial.printf("  temp           Reads the temperature.\n");
    Serial.printf("  cal cell mV    Calibrate the given cell.\n");
    Serial.printf("  param\n");
    Serial.printf("        save     Writes the data from RAM to EEPROM.\n");
    Serial.printf("        clear    Set the RAM data to zero.\n");
    Serial.printf("        discard  Wipes the EEPROM data.\n");
    Serial.printf("  reset          Resets the CPU.\n");
    Serial.printf("  ver            Prints version infos.\n");
    Serial.printf("  help           Prints this text.\n");

    return 0;
}

int8_t cmd_bat(char *argv[], uint8_t argc)
{
    int8_t numCells = lipo.getNumCells();
    
    numCells = LIPO_ADCCHANNELS;

    if (numCells < 0)
    {
        Serial.printf("Error: no LiPo pack detected!\n");
        return 0;
    }

    Serial.printf("Vref: %lu\n", lipo.getVref());
    for (uint8_t i = 0; i < numCells; i++)
    {
        uint32_t millis = lipo.getCell(i, true);
        Serial.printf("%d: %lu,%luV\n", i, millis/1000, millis%1000);
    }
    
    return 0;
}

int8_t cmd_cal(char *argv[], uint8_t argc)
{
    uint8_t cell = 0;
    uint32_t voltage = 0;

    if(argc != 2)
    {
        return -1;    
    }
    
    cell = strtol(argv[0], 0 ,0);
    voltage = strtol(argv[1], 0 ,0);
    lipo.calibrate(cell, voltage);

    return 0;
}

int8_t cmd_param(char *argv[], uint8_t argc)
{
    if(argc == 0)
    {
        return -1;
    }

    if(strcmp(argv[0],"clear") == 0)
    {
        param.clear();
        Serial.printf("Parameter clear in RAM.\n");
    }
    else if(strcmp(argv[0],"save") == 0)
    {
        param.save();
        Serial.printf("Parameter saved.\n");
    }
    else if(strcmp(argv[0],"discard") == 0)
    {
        param.discard();
        Serial.printf("Parameter discarded.\n");
    }
    else
    {
        Serial.printf("Invalid parameter.\n");
        return -1;
    }

    return 0;
}

int8_t cmd_temp(char *argv[], uint8_t argc)
{
    uint32_t temp = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(
            AVG_SLOPE, V25, CALX_TEMP, lipo.getVref(), analogRead(ATEMP), 
            LL_ADC_RESOLUTION_12B);

    Serial.printf("CPU: %lu°C\n", temp);

    return 0; 
}

int8_t cmd_reset(char *argv[], uint8_t argc)
{
    Serial.printf("Resetting the CPU...\n");
    delay(100);

    NVIC_SystemReset();

    return 0;
}

cliCmd_t cmdTable[] =
{
    {"help", cmd_help},
    {"ver", cmd_ver},
    {"bat", cmd_bat},
    {"temp", cmd_temp},
    {"cal", cmd_cal},
    {"param", cmd_param},
    {"reset", cmd_reset},
};

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    analogReadResolution(12);

    Serial.begin(115200);
    while (!Serial);   

    Serial.println();
    cmd_ver(0, 0);

    if(param.read() != true)
    {
        param.data.Battery = LIPO_PARAMS;
        param.save();
        Serial.printf("Parameter reset.\n");
    }
    else
    {
        Serial.printf("Parameter loaded.\n");
    }

    cli.begin(cmdTable);
}

void loop()
{
    static uint32_t lastTick = 0;
    uint32_t tick = millis();

    lipo.task(tick);

    if (tick - lastTick >= 250)
    {
        lastTick = tick;
        digitalToggle(LED_BUILTIN);
    }

    cli.read();
}