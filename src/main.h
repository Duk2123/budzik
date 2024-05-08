#ifndef MAIN_H
#define MAIN_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

extern TFT_eSPI tft;

extern int prevBrightness;
extern int brightness;
void setBrightness(int value);

int degToDirection(int degrees);

uint16_t hexToColor(String hex);

extern TaskHandle_t updateScreenElement_t;
extern SemaphoreHandle_t tftMutex;
extern TaskHandle_t handlePopup_t;

#endif