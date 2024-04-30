#include <main.h>
#include <screens/screens.h>

ScreenObject ClockScreen({{-999, 999}}, {{0, -390}}, {},
                         {displaySleep}, {menuScreen}, {});

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
    if (updateDisplay_t != NULL && eTaskGetState(updateDisplay_t) != 4)
    {
        vTaskDelete(updateDisplay_t);
    }
    activeScreenElement = &ClockScreen;
    tft.fillScreen(0);
    delay(100);
    clockSprite.createSprite(360, 72);
    clockSprite.setColorDepth(8);
    clockSprite.setTextColor(TFT_WHITE);
    clockSprite.setTextSize(10);

    xTaskCreate(updateClock, "updateClock", 2048, NULL, 2, &updateDisplay_t);
}
