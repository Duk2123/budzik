#ifndef NETWORK_H
#define NETWORK_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

extern NTPClient timeClient;

extern TaskHandle_t connectToNetwork_t;
void connectToNetwork(void *params);

#endif