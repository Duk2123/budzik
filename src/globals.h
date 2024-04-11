#ifndef GLOBALS_H
#define GLOBALS_H
#include <SPI.h>
#include <TFT_eSPI.h>
#include <vector>
#include <map>

extern TFT_eSPI tft;

class Screen;
extern Screen screens[2];
extern String activeScreen;

void menuScreen();
void clockScreen();

#endif