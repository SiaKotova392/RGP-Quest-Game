#ifndef GLOBAL_H
#define GLOBAL_H

#define HEIGHT 50
#define WIDTH 50
#define NUM_BUCKETS 50

// Include all the hardware libraries
#include "mbed.h"
#include "wave_player.h"
#include "MMA8452.h"
#include "uLCD_4DGL.h"
#include "SDFileSystem.h"

static int omnipotent;

// Declare the hardware interface objects
/*
This file holds all the global variables that are accessable from all the 
different files in this project. You see below most of it looks like hardware.
That's because it is hardware! The uLCD object is how you interact with the LCD
screen. The sd variable is how you interact with the sd card and so on for all
the other variables.
*/
extern uLCD_4DGL uLCD;      // LCD Screen
extern SDFileSystem sd;     // SD Card
extern Serial pc;           // USB Console output
extern MMA8452 acc;       // Accelerometer
extern DigitalIn button1;   // Pushbuttons
extern DigitalIn button2;
extern DigitalIn button3;
extern AnalogOut DACout;    // Speaker
extern PwmOut speaker;
extern wave_player waver;

// === [define the macro of error heandle function] ===
// when the condition (c) is not true, assert the program and show error code
#define ASSERT_P(c,e) do { \
    if(!(c)){ \
        pc.printf("\nERROR:%d\n",e); \
        while(1); \
    } \
} while (0)

// === [error code] ===
#define ERROR_NONE 0 // All good in the hood
#define ERROR_MEH -1 // This is how errors are done

#endif //GLOBAL_H