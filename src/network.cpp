#include <network.h>
#include <main.h>

int offset = 2; // TODO zrobić automatyczną zmiane stref czasowej

WiFiUDP ntpUDP;
HTTPClient http;
NTPClient timeClient(ntpUDP, "0.pl.pool.ntp.org", 3600 * offset, 28800000);

String networkSSID;
String networkPassword;

bool lostConnection = false;

TaskHandle_t WiFiConnectedEvent_t;
/** @brief Task for WiFi connected event */
void WiFiConnectedEvent(void *params)
{
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }
    lostConnection = false;
    timeClient.begin();
    timeClient.update();
    Serial.println("Connected");
    vTaskNotifyGiveFromISR(updateScreenElement_t, NULL);
    vTaskDelete(NULL);
}

/** @brief WiFi disconnected event*/
void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    if (!lostConnection)
    {
        lostConnection = true;
        int i = 0;
        timeClient.end();
        Serial.println("Disconnected");
        delay(10000);
        if ((connectToNetwork_t == NULL || (WiFiConnectedEvent_t != NULL && eTaskGetState(WiFiConnectedEvent_t) == 4)) && networkSSID != "")
            xTaskCreate(connectToNetwork, "connectToNetwork", 20048, NULL, 1, &connectToNetwork_t); // TODO zrobić taska który będzie próbował połączyć się z siecią z którą stracono połączenie
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
    Serial.println("Trying to connect");
    if (networkSSID != "")
    {
        Serial.println(networkSSID + " | " + networkPassword);
        WiFi.begin(networkSSID, networkPassword);
    }
    else
        Serial.println("No SSID"); // TODO error
    vTaskDelete(NULL);
}

/**
 * @brief queries and returns current weather of chosen location
 * @param lat geographic latitude of chosen location
 * @param lon geographic longitude of chosen location
 * @return CurrentWeather
 */
CurrentWeather getCurrentWeather(float lat, float lon)
{
    CurrentWeather temp;
    JsonDocument doc;
    char buffer[500];
    sprintf(buffer, "https://api.openweathermap.org/data/2.5/weather?lat=%f&lon=%f&units=metric&appid=6c058e95528f27729d1eee3888ce9691", lat, lon);
    String serverPath = String(buffer);
    if (WiFi.status() == WL_CONNECTED)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {

            http.begin(serverPath.c_str());
            int httpResponseCode = http.GET();

            if (httpResponseCode > 0)
            {
                String response = http.getString();
                deserializeJson(doc, response);
                temp = {convertSpecialLetters(doc["name"]), int(doc["weather"][0]["id"]),
                        int(doc["main"]["temp"]),
                        int(doc["main"]["feels_like"]),
                        int(doc["main"]["pressure"]),
                        int(doc["main"]["humidity"]),
                        int(doc["wind"]["speed"])};
            }
            else
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
                throw RequestError();
            }
            http.end();
        }
        delay(8);
        xSemaphoreGive(tftMutex);
        return temp;
    }
    else
    {
        throw ConnectionError();
    }
}

/**
 * @brief queries and returns current forecast of chosen location
 * @param lat geographic latitude of chosen location
 * @param lon geographic longitude of chosen location
 * @return CurrentForecast
 */
CurrentForecast getForecast(float lat, float lon)
{
    CurrentForecast temp;
    std::vector<int> tempId;
    std::vector<int> tempTemp;
    std::vector<int> tempDt;
    JsonDocument doc;

    char buffer[500];
    sprintf(buffer, "https://api.openweathermap.org/data/2.5/forecast?lat=%f&lon=%f&units=metric&cnt=8&appid=6c058e95528f27729d1eee3888ce9691", lat, lon);
    String serverPath = String(buffer);
    if (WiFi.status() == WL_CONNECTED)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            http.begin(serverPath.c_str());
            int httpResponseCode = http.GET();
            if (httpResponseCode > 0)
            {
                String response = http.getString();
                deserializeJson(doc, response);
                for (int i = 0; i < 8; i++)
                {
                    tempId.push_back(int(doc["list"][i]["weather"][0]["id"]));
                    tempTemp.push_back(int(doc["list"][i]["main"]["temp"]));
                    tempDt.push_back(int(doc["list"][i]["dt"]));
                }
                temp = {
                    (int(doc["list"][0]["pop"]) + int(doc["list"][1]["pop"])) / 2, tempId, tempTemp, tempDt};
            }
            else
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode); // TODO error
                throw RequestError();
            }
            http.end();
        }
        delay(8);
        xSemaphoreGive(tftMutex);
        return temp;
    }
    else
    {
        throw ConnectionError();
    }
}

/**
 * @brief queries and returns coordinate of chosen location
 * @param city String with name of a city
 * @param countryCode String with code of a country
 * @return <lat,lon>
 */
std::pair<float, float> getLocalization(String city, String countryCode)
{
    JsonDocument doc;
    char buffer[500];
    sprintf(buffer, "https://api.openweathermap.org/geo/1.0/direct?q=%s,%s&limit=1&appid=6c058e95528f27729d1eee3888ce9691", city, countryCode);
    float lat, lon;

    String serverPath = String(buffer);
    if (WiFi.status() == WL_CONNECTED)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
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
                throw RequestError();
            }
            http.end();
        }
        delay(8);
        xSemaphoreGive(tftMutex);
        return {lat, lon};
    }
    else
    {
        throw ConnectionError();
    }
}
