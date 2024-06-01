#include <screens/screens.h>
#include <network.h>
#include <climateSensor.h>
#include <time.h>

TFT_eSprite weatherBackground = TFT_eSprite(&tft);
TFT_eSprite weatherLocalization = TFT_eSprite(&tft);
TFT_eSprite weatherInfo = TFT_eSprite(&tft);
TFT_eSprite roomInfo = TFT_eSprite(&tft);
TFT_eSprite weatherForecast = TFT_eSprite(&tft);
TFT_eSprite weatherPages = TFT_eSprite(&tft);

std::vector<String> localizations = {"ROOM"};

int currentLocalization = 0;
CurrentWeather currentWeather;
CurrentForecast currentForecast;
std::pair<float, float> currentCoords;

std::vector<std::pair<float, float>> coordsArray = {{0, 0}};
std::vector<CurrentWeather> weatherArray = {currentWeather};
std::vector<CurrentForecast> forecastArray = {currentForecast};
uint32_t updateTime;

void drawLocalization();
void drawWeatherInfo();
void drawWeatherForecast();
void drawWeatherPages();
void drawRoomInfo();

void weatherChangePage()
{
    detectTouchSuspendCounter = 4;
    delay(16);
    if (degToDirection(touchCurrentAction[5]) == 2)
        currentLocalization = currentLocalization == 0 ? localizations.size() - 1 : currentLocalization - 1;
    else if (degToDirection(touchCurrentAction[5]) == 4)
        currentLocalization = currentLocalization == localizations.size() - 1 ? 0 : currentLocalization + 1;
    else if (degToDirection(touchCurrentAction[5]) == 1)
    {
        detectTouchSuspendCounter = 4;
        vTaskDelete(updateScreenElement_t);
        delay(16);

        weatherBackground.deleteSprite();
        weatherLocalization.deleteSprite();
        weatherInfo.deleteSprite();
        roomInfo.deleteSprite();
        weatherForecast.deleteSprite();
        weatherPages.deleteSprite();
        delay(8);
        clockScreen();
        return;
    }
    else
        return;

    currentWeather = weatherArray[currentLocalization];
    currentForecast = forecastArray[currentLocalization];
    drawLocalization();
    if (currentLocalization == 0)
    {
        drawRoomInfo();
    }
    else
    {
        drawWeatherInfo();
        drawWeatherForecast();
    }
    drawWeatherPages();
}

void weatherGoToMenu()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        detectTouchSuspendCounter = 4;
        vTaskDelete(updateScreenElement_t);
        delay(16);

        weatherBackground.deleteSprite();
        weatherLocalization.deleteSprite();
        weatherInfo.deleteSprite();
        roomInfo.deleteSprite();
        weatherForecast.deleteSprite();
        weatherPages.deleteSprite();

        delay(8);
        menuScreen();
    }
    else
        weatherChangePage();
}

bool weatherKeyboardActive = false;

void addLocalization()
{
    if (localizations.size() < 5)
    {
        weatherKeyboardActive = true;
        xTaskCreate(keyboardPopUp, "keyboardPopUp", 4096, NULL, 3, &handlePopup_t);
    }
    // TODO komunikat/błąd
}

void removeLocalization()
{
    Serial.println("removeLocalization");
    if (currentLocalization != 0)
    {
        localizations.erase(localizations.begin() + currentLocalization);
        coordsArray.erase(coordsArray.begin() + currentLocalization);
        weatherArray.erase(weatherArray.begin() + currentLocalization);
        forecastArray.erase(forecastArray.begin() + currentLocalization);
        Serial.println("removed");
        currentLocalization -= 1;

        drawLocalization();
        if (currentLocalization == 0)
        {
            drawRoomInfo();
        }
        else
        {
            drawWeatherInfo();
            drawWeatherForecast();
        }
        drawWeatherPages();
    }
    // TODO komunikat/błąd
}

ScreenObject WeatherScreen({{0, 0, 480, 360}}, {{0, 0, 480, 160}, {0, 0, 480, 360}}, {{64, 58, 94, 88}, {386, 58, 416, 88}},
                           {displaySleep}, {weatherGoToMenu, weatherChangePage}, {removeLocalization, addLocalization});

void updateWeather()
{
    uint32_t temp = rtc.now().unixtime();
    if (temp - updateTime > 900)
    {
        Serial.println("Update weather");
        updateTime = temp;
        for (int i = 1; i < localizations.size(); i++)
        {
            weatherArray[i] = getCurrentWeather(coordsArray[i].first, coordsArray[i].second);
            forecastArray[i] = getForecast(coordsArray[i].first, coordsArray[i].second);
        }
    }
}

void updateWeatherScreen(void *params)
{
    for (;;)
    {
        if (weatherKeyboardActive && ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 0)
        {
            weatherKeyboardActive = false;
            if (keyboardInput != "")
            {
                xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
                {
                    weatherBackground.fillSprite(BLACK);
                    weatherBackground.pushSprite(0, 40);
                }
                delay(8);
                xSemaphoreGive(tftMutex);

                currentCoords = getLocalization(keyboardInput, "PL");
                try
                {
                    currentWeather = getCurrentWeather(currentCoords.first, currentCoords.second);
                    currentForecast = getForecast(currentCoords.first, currentCoords.second);

                    coordsArray.push_back(currentCoords);
                    weatherArray.push_back(currentWeather);
                    localizations.push_back(currentWeather.name);
                    currentLocalization = localizations.size() - 1;

                    forecastArray.push_back(currentForecast);
                }
                catch (const std::exception &e)
                {
                    Serial.println(e.what()); // TODO
                }
            }

            drawLocalization();
            drawWeatherInfo();
            drawWeatherForecast();
            drawWeatherPages();
        }
        vTaskDelay(250);
    }
}

void drawLocalization()
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        weatherLocalization.fillSprite(BLACK);
        weatherLocalization.fillRect(0, 16, 30, 2, WHITE);
        weatherLocalization.fillRect(322, 16, 30, 2, WHITE);
        weatherLocalization.fillRect(336, 2, 2, 30, WHITE);
        weatherLocalization.drawCentreString(localizations[currentLocalization], 176, 4, 4);
        weatherLocalization.pushSprite(64, 56);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawWeatherInfo()
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        weatherInfo.fillSprite(BLACK);
        weatherInfo.setTextDatum(2);
        weatherInfo.setTextSize(3);
        weatherInfo.drawString(String(currentWeather.temp) + "C", 182, 16, 4);
        weatherInfo.setTextSize(1);
        weatherInfo.setTextDatum(0);

        weatherInfo.fillRect(198, 18, 30, 30, WHITE);
        weatherInfo.drawString(String("PoP"), 198, 48, 4);
        weatherInfo.drawString(String(currentForecast.pop) + "%", 264, 48, 4);

        weatherInfo.drawString("Humidity", 310, 0, 2);
        weatherInfo.drawString("Real feel", 310, 24, 2);
        weatherInfo.drawString("Sea level", 310, 48, 2);
        weatherInfo.drawString("Wind", 310, 72, 2);

        weatherInfo.drawString(String(currentWeather.humidity) + "%", 372, 0, 2);
        weatherInfo.drawString(String(currentWeather.feelsLike) + "C", 372, 24, 2);
        weatherInfo.drawString(String(currentWeather.pressure) + " hPa", 372, 48, 2);
        weatherInfo.drawString(String(currentWeather.windSpeed) + " Km/h", 372, 72, 2);

        weatherInfo.pushSprite(16, 106);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawRoomInfo()
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        roomInfo.fillSprite(BLACK);

        roomInfo.setTextSize(1);
        roomInfo.drawCentreString("Temperature", 70, 16, 4);
        roomInfo.drawCentreString("Humidity", 224, 16, 4);
        roomInfo.drawCentreString("Ground level", 378, 16, 4);

        roomInfo.setTextSize(2);
        roomInfo.drawCentreString(String(int(round(bme.readTemperature()))), 70, 64, 4);
        roomInfo.drawCentreString(String(int(round(bme.readHumidity()))), 224, 64, 4);
        roomInfo.drawCentreString(String(int(bme.readPressure() / 100)), 378, 64, 4);

        roomInfo.drawCentreString("C", 70, 136, 4);
        roomInfo.drawCentreString("%", 224, 136, 4);
        roomInfo.drawCentreString("hPa", 378, 136, 4);

        roomInfo.pushSprite(16, 106);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawWeatherForecast()
{
    char buffer[50];
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        weatherForecast.fillSprite(BLACK);

        for (int i = 0; i < 8; i++)
        {
            sprintf(buffer, "%d", currentForecast.temp[i]);
            weatherForecast.drawCentreString(String(buffer), 20 + 56 * i, 0, 2);
            weatherForecast.fillRect(5 + 56 * i, 24, 30, 30, WHITE); // TODO ikony
            DateTime dt(currentForecast.dt[i]);
            sprintf(buffer, "%02d:00", dt.hour());
            weatherForecast.drawCentreString(String(buffer), 20 + 56 * i, 62, 2);
        }

        weatherForecast.pushSprite(22, 218);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawWeatherPages()
{
    int size = localizations.size();
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        weatherPages.fillSprite(BLACK);

        for (int i = 0; i < size; i++)
        {
            weatherPages.fillCircle(4 + 8 * (6 - size) + 16 * i, 5, 4, i == currentLocalization ? WHITE : hexToColor("5D5E5F"));
        }

        weatherPages.pushSprite(196, 302);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void weatherScreen()
{
    detectTouchSuspendCounter = 4;
    ActiveScreenElement = &WeatherScreen;
    currentLocalization = 0;
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        weatherBackground.createSprite(480, 320);
        weatherBackground.fillSprite(BLACK);
        weatherBackground.pushSprite(0, 40);

        weatherLocalization.createSprite(352, 34);
        weatherInfo.createSprite(448, 112);
        roomInfo.createSprite(448, 192);
        weatherForecast.createSprite(448, 76);
        weatherPages.createSprite(88, 10);

        weatherLocalization.setTextColor(WHITE);
        weatherInfo.setTextColor(WHITE);
        weatherForecast.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    if (localizations.size() > 1)
        updateWeather();

    drawLocalization();
    drawRoomInfo();
    drawWeatherPages();

    xTaskCreate(updateWeatherScreen, "updateWeatherScreen", 4096, NULL, 3, &updateScreenElement_t);
}