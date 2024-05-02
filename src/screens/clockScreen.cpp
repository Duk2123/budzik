#include <main.h>
#include <screens/screens.h>

void clockScreenToMenuScreen()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
        menuScreen();
}

ScreenObject ClockScreen({{0, 0, 480, 360}}, {{0, 0, 480, 360}}, {},
                         {displaySleep}, {clockScreenToMenuScreen}, {});

void updateClock(void *params)
{
    Serial.println("task");
    int i = 0;
    for (;;)
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            clockSprite.fillSprite(TFT_BLACK);
            clockSprite.setTextColor(TFT_WHITE);
            clockSprite.drawString(String(i), 0, 0);
            clockSprite.pushSprite(60, 144);
            i++;
        }
        vTaskDelay(7);
        xSemaphoreGive(tftMutex);
        vTaskDelay(100);
    }
}

void clockScreen()
{
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskDelete(updateScreenElement_t);
    }
    activeScreenElement = &ClockScreen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        tft.fillScreen(0);
        delay(100);
        clockSprite.createSprite(360, 72);
        clockSprite.setColorDepth(8);
        clockSprite.setTextColor(TFT_WHITE);
        clockSprite.setTextSize(10);
    }
    xSemaphoreGive(tftMutex);
    xTaskCreate(updateClock, "updateClock", 2048, NULL, 2, &updateScreenElement_t);
}
