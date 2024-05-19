#include <screens/screens.h>
#include <network.h>

TFT_eSprite weatherBackground = TFT_eSprite(&tft);
TFT_eSprite weatherLocalization = TFT_eSprite(&tft);
TFT_eSprite weatherInfo = TFT_eSprite(&tft);
TFT_eSprite weatherForecast = TFT_eSprite(&tft);
TFT_eSprite weatherPages = TFT_eSprite(&tft);

String currentLocalization = "ADD NEW";
String currentWeather;
String currentForecast;
String id, temp, rain, humidity, real, pressure, wind;
std::pair<float, float> currentCoords;

void weatherChangePage() {}

void weatherGoToMenu()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        detectTouchSuspendCounter = 4;
        delay(16);

        weatherBackground.deleteSprite();
        weatherLocalization.deleteSprite();
        weatherInfo.deleteSprite();
        weatherForecast.deleteSprite();
        weatherPages.deleteSprite();

        delay(8);
        menuScreen();
    }
    else
        weatherChangePage();
}

bool weatherKeyboardActive = false;

void weatherKeyboard()
{
    weatherKeyboardActive = true;
    xTaskCreate(keyboardPopUp, "keyboardPopUp", 20048, NULL, 3, &handlePopup_t);
}

ScreenObject WeatherScreen({{0, 0, 480, 360}}, {{0, 0, 480, 160}, {0, 0, 480, 360}}, {{386, 58, 416, 88}},
                           {displaySleep}, {weatherGoToMenu, weatherChangePage}, {weatherKeyboard});

void drawLocalization();
void drawWeatherInfo();

void updateWeather(void *params)
{

    for (;;)
    {
        if (weatherKeyboardActive)
        {
            vTaskDelay(250);
        }
        else
        {
            if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 0)
            {
                weatherKeyboardActive = false;
                xSemaphoreTake(tftMutex, portMAX_DELAY);
                {
                    weatherBackground.fillSprite(BLACK);
                    weatherBackground.pushSprite(0, 40);
                }
                delay(8);
                xSemaphoreGive(tftMutex);
                currentLocalization = keyboardInput;
                currentCoords = getLocalization(currentLocalization, "PL");
                drawLocalization();
                currentWeather = getCurrentWeather(currentCoords.first, currentCoords.second);
                currentForecast = getForecast(currentCoords.first, currentCoords.second);
                Serial.println(currentWeather);
                Serial.println(currentForecast);

                drawWeatherInfo();
            }

            vTaskDelay(250);
        }
    }
}

void drawLocalization()
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        weatherLocalization.fillSprite(BLACK);
        weatherLocalization.fillRect(0, 16, 30, 2, WHITE);
        weatherLocalization.fillRect(322, 16, 30, 2, WHITE);
        weatherLocalization.fillRect(336, 2, 2, 30, WHITE);
        weatherLocalization.drawCentreString(currentLocalization, 240, 4, 4);
        weatherLocalization.pushSprite(64, 56);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawWeatherInfo()
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        weatherInfo.fillSprite(BLACK);
        weatherInfo.setTextDatum(2);
        weatherInfo.setTextSize(2);
        weatherInfo.drawString(temp, 162, 4, 4);
        weatherInfo.setTextSize(1);
        weatherInfo.setTextDatum(0);

        weatherInfo.fillRect(194, 6, 30, 30, WHITE);
        weatherInfo.fillRect(194, 36, 30, 30, WHITE);
        weatherInfo.drawString(rain, 224, 36, 4);

        weatherInfo.drawString("Humidity", 286, 0, 2);
        weatherInfo.drawString("Real feel", 286, 24, 2);
        weatherInfo.drawString("Sea level", 286, 48, 2);
        weatherInfo.drawString("Wind", 286, 72, 2);

        weatherInfo.drawString(humidity, 346, 0, 2);
        weatherInfo.drawString(real, 346, 24, 2);
        weatherInfo.drawString(pressure, 346, 48, 2);
        weatherInfo.drawString(wind, 346, 72, 2);

        weatherInfo.pushSprite(32, 118);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawWeatherForecast()
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        weatherForecast.fillSprite(BLACK);

        for (int i = 0; i < 10; i++)
        {
            weatherForecast.drawCentreString("17", 20 + 56 * i, 0, 2);
            weatherForecast.fillRect(5 + 56 * i, 24, 30, 30, WHITE);
            weatherForecast.drawCentreString("00:00", 20 + 56 * i, 62, 2);
        }

        weatherForecast.pushSprite(22, 218);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawWeatherPages()
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        weatherPages.fillSprite(BLACK);

        for (int i = 0; i < 2; i++)
        {
            weatherPages.fillCircle(4 + 16 * i, 4, 4, WHITE);
        }

        weatherPages.pushSprite(22, 304);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void weatherScreen()
{
    detectTouchSuspendCounter = 4;
    activeScreenElement = &WeatherScreen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        weatherBackground.createSprite(480, 320);
        weatherBackground.fillSprite(BLACK);
        weatherBackground.pushSprite(0, 40);

        weatherLocalization.createSprite(352, 34);
        weatherInfo.createSprite(408, 72);
        weatherForecast.createSprite(448, 77);
        weatherPages.createSprite(240, 8);

        weatherLocalization.setTextColor(WHITE);
        weatherInfo.setTextColor(WHITE);
        weatherForecast.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawLocalization();
    drawWeatherInfo();
    drawWeatherForecast();
    drawWeatherPages();

    xTaskCreate(updateWeather, "updateWeather", 20048, NULL, 3, &updateScreenElement_t);
}