#include <main.h>
#include <screens/screens.h>
#include <time.h>

TFT_eSprite clockSprite = TFT_eSprite(&tft);

int activeMode = 0;

void drawClock(String time);
void changeMode();

void goToMenu()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        menuScreen();
    }
    else
        changeMode();
}

void changeMode()
{
    vTaskSuspend(detectTouch_t);
    if (degToDirection(touchCurrentAction[5]) == 2)
        activeMode = activeMode == 0 ? 1 : activeMode - 1;
    else if (degToDirection(touchCurrentAction[5]) == 4)
        activeMode = activeMode == 1 ? 0 : activeMode + 1;
    else
        return;

    String time = getRtcTime();
    drawClock(time);
    vTaskResume(detectTouch_t);
}

ScreenObject ClockScreen({{0, 0, 480, 360}}, {{0, 0, 480, 160}, {0, 0, 480, 360}}, {},
                         {displaySleep}, {goToMenu, changeMode}, {});

void drawClock(String time)
{
    String date = getRtcDate();
    char buffer[50];
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        switch (activeMode)
        {
        case 0:
            clockSprite.fillSprite(TFT_BLACK);
            clockSprite.drawString(String(time), 64, 128); // TODO zmienić font
            clockSprite.pushSprite(0, 0);
            break;
        case 1:
            sprintf(buffer, "%s, %s %s", date.substring(0, date.indexOf(".")), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth());
            clockSprite.fillSprite(TFT_BLACK);
            clockSprite.drawString(time, 64, 96);
            clockSprite.setTextSize(2);
            clockSprite.drawString(String(buffer), 64, 168);
            clockSprite.setTextSize(10);
            clockSprite.pushSprite(0, 0); // TODO zmienić środkowanie
            break;
        default:
            break;
        }
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void updateClock(void *params)
{
    String prevTime = "00:00:00";
    String time;
    clockSprite.setTextColor(TFT_WHITE);
    for (;;)
    {
        time = getRtcTime();
        if (time != prevTime)
        {
            prevTime = time;
            drawClock(time);
        }
        else
            vTaskDelay(125);
    }
}

void clockScreen()
{
    vTaskSuspend(detectTouch_t);
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskDelete(updateScreenElement_t);
    }
    activeScreenElement = &ClockScreen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        tft.fillScreen(0);
        delay(32);
        clockSprite.createSprite(480, 360);
        clockSprite.setColorDepth(16);
        clockSprite.setTextColor(TFT_WHITE);
        clockSprite.setTextSize(10);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
    xTaskCreate(updateClock, "updateClock", 20048, NULL, 2, &updateScreenElement_t); // TODO pamięć
    vTaskResume(detectTouch_t);
}
