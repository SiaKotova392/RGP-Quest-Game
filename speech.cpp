#include "speech.h"

#include "globals.h"
#include "hardware.h"

#define PURPLE 0x800080

/**
 * Draw the speech bubble background.
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP 0
#define BOTTOM 1
static void draw_speech_line(const char *line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.filled_rectangle(3, 93, 123, 113, GREEN);
    uLCD.filled_rectangle(4, 94, 122, 112, PURPLE);
}

void erase_speech_bubble()
{
}

void draw_speech_line(const char *line, int which)
{
}

int readPB1(int pb1)
{
    GameInputs inputs;
    inputs = read_inputs();
    if (inputs.b1 == 0)
        pb1 = 0;
    return pb1;
}

void speech_bubble_wait()
{
    int pb1 = 1;
    while (pb1 == 1) {
        for (int i = 0; i < 4; i++) {
            uLCD.filled_circle(120, 15, 4, RED);
            wait(0.1);
            pb1 = readPB1(pb1);
        }

        if (pb1 == 0)
            break;

        for (int i = 0; i < 4; i++) {
            uLCD.filled_circle(120, 15, 4, BLACK);
            wait(0.1);
            pb1 = readPB1(pb1);
        }
    }
}

void speech(const char *line)
{
    uLCD.textbackground_color(PURPLE);
    int i;
    while (*line) {
        draw_speech_bubble();

        i = 0;
        uLCD.locate(1, 12);
        while(i < 15 && *line) {
            uLCD.printf("%c", *line);
            wait(0.05);
            i++;
            line++;
        }

        if(!*line) {
            speech_bubble_wait();
            break;
        }

        uLCD.locate(1, 13);
        while(i < 30 && *line) {
            uLCD.printf("%c", *line);
            wait(0.05);
            i++;
            line++;
        }
        speech_bubble_wait();
    }
}

void long_speech(const char *lines[], int n)
{
}
