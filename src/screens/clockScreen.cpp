#include <main.h>
#include <screens/screens.h>
#include <time.h>
#include <climateSensor.h>

TFT_eSprite clockSprite = TFT_eSprite(&tft);

int activeMode = 0;

void drawClock(String time);
void changeMode();

void goToMenu()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        detectTouchSuspendCounter = 4;
        delay(16);
        vTaskDelete(updateScreenElement_t);
        clockSprite.deleteSprite();
        menuScreen();
    }
    else
        changeMode();
}

void changeMode()
{
    detectTouchSuspendCounter = 3;
    if (degToDirection(touchCurrentAction[5]) == 2)
        activeMode = activeMode == 0 ? 2 : activeMode - 1;
    else if (degToDirection(touchCurrentAction[5]) == 4)
        activeMode = activeMode == 2 ? 0 : activeMode + 1;
    else
        return;

    String time = getRtcTime();
    drawClock(time);
}

ScreenObject ClockScreen({{0, 0, 480, 360}}, {{0, 0, 480, 120}, {0, 0, 480, 360}}, {},
                         {displaySleep}, {goToMenu, changeMode}, {});

void drawClock(String time)
{
    String date = getRtcDate();
    char buffer[50];
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    int temp;
    {
        switch (activeMode)
        {
        case 0:
            clockSprite.fillSprite(BLACK);
            time.remove(5, 3);
            clockSprite.setTextSize(3);
            clockSprite.drawCentreString(time, 240, 88, 7); // TODO zmienić font
            clockSprite.pushSprite(0, 0);
            break;
        case 1:
            sprintf(buffer, "%s, %s %s %s", date.substring(0, date.indexOf(".")), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth(), date.substring(date.indexOf(".") + 7, date.indexOf(".") + 11));
            clockSprite.fillSprite(BLACK);
            clockSprite.setTextSize(1);
            clockSprite.drawCentreString(time, 240, 92, 8);
            clockSprite.setTextSize(1);
            clockSprite.drawCentreString(String(buffer), 240, 220, 4);
            clockSprite.pushSprite(0, 0);
            break;
        case 2:
            sprintf(buffer, "%s, %s %s", date.substring(0, 3), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth());
            clockSprite.fillSprite(BLACK);
            clockSprite.setTextSize(3);
            time.remove(5, 3);
            clockSprite.drawCentreString(time, 240, 86, 6);
            clockSprite.setTextSize(1);
            clockSprite.drawString(String(buffer), 32, 242, 4);
            sprintf(buffer, "°C %2.0f", round(bme.readTemperature()));
            temp = clockSprite.drawString(String(buffer), 280, 242, 4); // TODO zmienic font
            sprintf(buffer, "%H %2.0f", round(bme.readHumidity()));
            clockSprite.drawString(String(buffer), 288 + temp, 242, 4); // TODO zmienic font
            clockSprite.pushSprite(0, 0);
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
    clockSprite.setTextColor(WHITE);
    for (;;)
    {
        time = getRtcTime();
        if (time != prevTime)
        {
            prevTime = time;
            drawClock(time);
        }
        else
            vTaskDelay(250);
    }
}

void clockScreen()
{
    detectTouchSuspendCounter = 4;
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskDelete(updateScreenElement_t);
    }
    activeScreenElement = &ClockScreen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        delay(8);
        tft.fillScreen(BLACK);
        delay(32);
        clockSprite.createSprite(480, 360); // TODO zmienic sprite
        // clockSprite.setColorDepth(16);
        clockSprite.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
    xTaskCreate(updateClock, "updateClock", 20048, NULL, 2, &updateScreenElement_t); // TODO pamięć
}
