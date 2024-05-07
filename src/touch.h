#ifndef TOUCH_H
#define TOUCH_H

#ifndef GLOBALS_H
#include <globals.h>
#endif

typedef struct
{
    int x;
    int y;
} coordinates;

extern int touchCurrentAction[6];
extern QueueHandle_t touchQueue;

extern TaskHandle_t handleTouch_t;
extern TaskHandle_t detectTouch_t;

extern bool isTouchProcessing;
void handleTouch(void *params);
extern int detectTouchSuspendCounter;
void detectTouch(void *params);

#endif