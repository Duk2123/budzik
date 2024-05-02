#include <main.h>
#include <screens/screens.h>
#include <time.h>

void clockScreenHandleSwipe()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
        menuScreen();
    else if (degToDirection(touchCurrentAction[5]) == 1)
    {
        syncRtcToNtp();
    }
}

ScreenObject ClockScreen({{0, 0, 480, 360}}, {{0, 0, 480, 360}}, {},
                         {displaySleep}, {clockScreenHandleSwipe}, {});

void updateClock(void *params)
{
    int16_t width;
    String prevTime = "00:00:00";
    String time;
    for (;;)
    {
        time = getRtcTime();
        if (time != prevTime)
        {
            prevTime = time;
            xSemaphoreTake(tftMutex, portMAX_DELAY);
            {
                clockSprite.fillSprite(TFT_BLACK);
                clockSprite.setTextColor(TFT_WHITE);
                width = clockSprite.drawString(String(time), 0, 0); // TODO zmienić font
                clockSprite.pushSprite((480 - width) / 2, 144);
            }
            vTaskDelay(8);
            xSemaphoreGive(tftMutex);
        }
        else
            vTaskDelay(125);
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
        clockSprite.createSprite(480, 72);
        clockSprite.setColorDepth(8);
        clockSprite.setTextColor(TFT_WHITE);
        clockSprite.setTextSize(10);
    }
    xSemaphoreGive(tftMutex);
    xTaskCreate(updateClock, "updateClock", 2048, NULL, 2, &updateScreenElement_t); // TODO przesunąć to do funkcji przejściowej (clockScreenToMenuScreen)
}
