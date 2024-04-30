#ifndef MAIN_H
#define MAIN_H

#ifndef GLOBALS_H
#include <globals.h>
#endif

extern TFT_eSPI tft;
extern TFT_eSprite clockSprite;
extern int prevBrightness;
extern int brightness;
void setBrightness(int value);
extern TaskHandle_t updateDisplay_t;
extern TaskHandle_t detectTouch_t;
extern SemaphoreHandle_t tftMutex;

#endif