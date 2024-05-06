#ifndef NETWORK_H
#define NETWORK_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

extern NTPClient timeClient;

extern TaskHandle_t connectToNetwork_t;
void connectToNetwork(void *params);
void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);

#endif