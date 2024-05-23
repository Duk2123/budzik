#include <network.h>
#include <main.h>

int offset = 2; // TODO zrobić automatyczną zmiane stref czasowej

WiFiUDP ntpUDP;
HTTPClient http;
NTPClient timeClient(ntpUDP, "0.pl.pool.ntp.org", 3600 * offset, 28800000);

const char *ssid = "JAVR"; // TODO
const char *password = "kebab1212";

TaskHandle_t WiFiConnectedEvent_t;
/** @brief Task for WiFi connected event */
void WiFiConnectedEvent(void *params)
{
    delay(5000);
    timeClient.begin();
    timeClient.update();
    Serial.println("Connected");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }
    vTaskDelete(NULL);
}

/** @brief WiFi disconnected event*/
void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    timeClient.end();
    Serial.println("Disconnected");
    xTaskCreate(connectToNetwork, "connectToNetwork", 20048, NULL, 1, &connectToNetwork_t); // TODO obciąć pamięć
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }
}

/** @brief WiFi connected event */
void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    while (WiFiConnectedEvent_t != NULL || (WiFiConnectedEvent_t != NULL && eTaskGetState(WiFiConnectedEvent_t) != 4))
        delay(1000);
    xTaskCreate(WiFiConnectedEvent, "WiFiConnectedEvent", 20048, NULL, 1, &WiFiConnectedEvent_t);
}

TaskHandle_t connectToNetwork_t;
/** @brief Task for connecting to WiFi*/
void connectToNetwork(void *params)
{
    WiFi.begin(ssid, password);

    int i = 0;

    while (WiFi.status() != WL_CONNECTED && i < 60)
    {
        vTaskDelay((1000 + pow((i++ - 1), 3)) < 60000 ? (1000 + pow((i++ - 1), 3)) : 60000);
    }

    if (i == 60)
        Serial.println("Failed to connect");

    vTaskDelete(NULL);
}

// Returns String with id, temp, feels_like, pressure, humidity, wind speed "%3d;%3d;%3d;%4d;%2d;%2d"
String getCurrentWeather(float lat, float lon)
{
    JsonDocument doc;
    char buffer[500];
    sprintf(buffer, "https://api.openweathermap.org/data/2.5/weather?lat=%f&lon=%f&units=metric&appid=6c058e95528f27729d1eee3888ce9691", lat, lon);
    String serverPath = String(buffer);
    if (WiFi.status() == WL_CONNECTED)
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            http.begin(serverPath.c_str());
            int httpResponseCode = http.GET();

            if (httpResponseCode > 0)
            {
                String response = http.getString();
                deserializeJson(doc, response);
                sprintf(buffer, "%3d;%3d;%3d;%4d;%2d;%2d",
                        int(doc["weather"][0]["id"]), int(doc["main"]["temp"]), int(doc["main"]["feels_like"]), int(doc["main"]["pressure"]), int(doc["main"]["humidity"]), int(doc["wind"]["speed"]));
            }
            else
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            http.end();
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        Serial.println("No connection");
    }
    return String(buffer);
}

// Returns a String pop;(dt,temp,id)*9 "%3d;%10d,%3d,%3d..."
String getForecast(float lat, float lon)
{
    JsonDocument doc;
    char buffer[2250];
    sprintf(buffer, "https://api.openweathermap.org/data/2.5/forecast?lat=%f&lon=%f&units=metric&appid=6c058e95528f27729d1eee3888ce9691", lat, lon);
    String serverPath = String(buffer);
    String temp;
    if (WiFi.status() == WL_CONNECTED)
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            http.begin(serverPath.c_str());
            int httpResponseCode = http.GET();

            if (httpResponseCode > 0)
            {
                String response = http.getString();
                deserializeJson(doc, response);
                sprintf(buffer, "%3d",
                        float(doc["list"][0]["pop"]) * 100);
                temp = String(buffer);
                for (int i = 0; i < 10; i++)
                {
                    sprintf(buffer, ";%10d,%3d,%3d", int(doc["list"][i]["dt"]), int(doc["list"][i]["main"]["temp"]), int(doc["list"][i]["weather"][0]["id"]));
                    temp += String(buffer);
                }
            }
            else
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            http.end();
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        Serial.println("No connection");
    }
    return temp;
}

std::pair<float, float> getLocalization(String city, String countryCode)
{
    JsonDocument doc;
    char buffer[500];
    sprintf(buffer, "https://api.openweathermap.org/geo/1.0/direct?q=%s,%s&limit=1&appid=6c058e95528f27729d1eee3888ce9691", city, countryCode);
    float lat, lon;

    String serverPath = String(buffer);
    if (WiFi.status() == WL_CONNECTED)
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            http.begin(serverPath.c_str());
            int httpResponseCode = http.GET();

            if (httpResponseCode > 0)
            {

                String response = http.getString();
                deserializeJson(doc, response);
                lat = doc[0]["lat"];
                lon = doc[0]["lon"];
            }
            else
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            http.end();
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        Serial.println("No connection");
    }
    return {lat, lon};
}