#ifndef NETWORK_H
#define NETWORK_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

#include <ArduinoJson.h>
#include <HTTPClient.h>

struct ConnectionError : public std::exception
{
    const char *what() const noexcept override
    {
        return "No wifi connection";
    }
};
struct RequestError : public std::exception
{
    const char *what() const noexcept override
    {
        return "Request error";
    }
};
typedef struct
{
    String name;
    int id;
    int temp;
    int feelsLike;
    int pressure;
    int humidity;
    int windSpeed;
} CurrentWeather;

typedef struct
{
    int pop;
    std::vector<int> id;
    std::vector<int> temp;
    std::vector<int> dt;
} CurrentForecast;

extern NTPClient timeClient;

extern TaskHandle_t connectToNetwork_t;
void connectToNetwork(void *params);
void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);

CurrentWeather getCurrentWeather(float lat, float lon);
CurrentForecast getForecast(float lat, float lon);
std::pair<float, float> getLocalization(String city, String countryCode);

extern String networkSSID;
extern String networkPassword;

#endif