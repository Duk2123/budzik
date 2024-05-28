#include <screens/screens.h>
#include <time.h>
#include <climateSensor.h>

TFT_eSprite ClockBackground = TFT_eSprite(&tft);
TFT_eSprite ClockHour = TFT_eSprite(&tft);
TFT_eSprite ClockDate = TFT_eSprite(&tft);

int activeMode = 1;

void drawClock(String time);
void changeMode();

void goToMenu()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        detectTouchSuspendCounter = 4;
        delay(16);
        vTaskDelete(updateScreenElement_t);

        ClockBackground.deleteSprite();
        ClockHour.deleteSprite();
        ClockDate.deleteSprite();

        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            statusBarWiFiActive = true;
            statusBarClockActive = true;
        }
        delay(32);
        xSemaphoreGive(tftMutex);

        drawWiFiStatus();
        drawStatusBarClock();

        delay(8);
        menuScreen();
    }
    else
        changeMode();
}

void changeMode()
{
    detectTouchSuspendCounter = 4;
    delay(16);
    if (degToDirection(touchCurrentAction[5]) == 2)
        activeMode = activeMode == 0 ? 2 : activeMode - 1;
    else if (degToDirection(touchCurrentAction[5]) == 4)
        activeMode = activeMode == 2 ? 0 : activeMode + 1;
    else
        return;

    String time = getRtcTime();

    delay(4);
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        ClockBackground.fillSprite(BLACK);
        ClockBackground.pushSprite(0, 40);
        if (activeMode == 0)
            statusBarWiFiActive = false;
        else
            statusBarWiFiActive = true;
    }
    delay(12);
    xSemaphoreGive(tftMutex);

    drawWiFiStatus();
    drawClock(time);
}

ScreenObject ClockScreen({{0, 0, 480, 360}}, {{0, 0, 480, 160}, {0, 0, 480, 360}}, {},
                         {displaySleep}, {goToMenu, changeMode}, {});

void drawClock(String time)
{
    String date = getRtcDate();
    char buffer[50];
    int temp;
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        switch (activeMode)
        {
        case 0:
            ClockHour.fillSprite(BLACK);
            time.remove(5, 3);
            ClockHour.setTextSize(3);
            ClockHour.drawCentreString(time, 240, 0, 7); // TODO zmienić font
            ClockHour.pushSprite(0, 88);
            break;
        case 1:
            ClockHour.fillSprite(BLACK);
            ClockHour.setTextSize(1);
            ClockHour.drawCentreString(time, 240, 34, 8);
            ClockHour.pushSprite(0, 88);

            sprintf(buffer, "%s, %s %s %s", date.substring(0, date.indexOf(".")), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth(), date.substring(date.indexOf(".") + 7, date.indexOf(".") + 11));
            ClockDate.fillSprite(BLACK);
            ClockDate.setTextSize(1);
            ClockDate.drawCentreString(String(buffer), 240, 0, 4);
            ClockDate.pushSprite(0, 232);
            break;
        case 2:
            ClockHour.fillSprite(BLACK);
            ClockHour.setTextSize(3);
            time.remove(5, 3);
            ClockHour.drawCentreString(time, 240, 16, 6);
            ClockHour.pushSprite(0, 88);

            sprintf(buffer, "%s, %s %s", date.substring(0, 3), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth());
            ClockDate.fillSprite(BLACK);
            ClockDate.setTextSize(1);
            ClockDate.drawString(String(buffer), 32, 0, 4);

            sprintf(buffer, "°C %2.0f", round(bme.readTemperature() - 1));
            ClockDate.drawString(String(buffer), 306, 0, 4); // TODO zmienic font

            sprintf(buffer, "%H %2.0f", round(bme.readHumidity()));
            ClockDate.drawString(String(buffer), 381, 0, 4); // TODO zmienic font
            ClockDate.pushSprite(0, 248);
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
    for (;;)
    {
        time = getRtcTime();
        if (time != prevTime)
        {
            prevTime = time;
            drawClock(time);
        }
        else
            vTaskDelay(activeMode == 1 ? 125 : 1000);
    }
}

void clockScreen()
{
    detectTouchSuspendCounter = 4;
    ActiveScreenElement = &ClockScreen;

    if (activeMode == 0)
        statusBarWiFiActive = false;
    drawWiFiStatus();

    statusBarClockActive = false;
    drawStatusBarClock();

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        ClockBackground.createSprite(480, 320);
        ClockBackground.fillSprite(BLACK);
        ClockBackground.pushSprite(0, 40);

        ClockHour.createSprite(480, 144);
        ClockDate.createSprite(480, 36);

        ClockHour.setTextColor(WHITE);
        ClockDate.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    xTaskCreate(updateClock, "updateClock", 20048, NULL, 3, &updateScreenElement_t); // TODO pamięć
}
