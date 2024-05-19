#include <screens/screens.h>
#include <time.h>
#include <climateSensor.h>

TFT_eSprite clockBackground = TFT_eSprite(&tft);
TFT_eSprite clockHour = TFT_eSprite(&tft);
TFT_eSprite clockDate = TFT_eSprite(&tft);

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

        clockBackground.deleteSprite();
        clockHour.deleteSprite();
        clockDate.deleteSprite();

        statusBarWiFiActive = true;
        drawWiFiStatus();

        statusBarClockActive = true;
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
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        clockBackground.fillSprite(BLACK);
        clockBackground.pushSprite(0, 40);
    }
    delay(12);
    xSemaphoreGive(tftMutex);

    if (activeMode == 0)
        statusBarWiFiActive = false;
    else
        statusBarWiFiActive = true;

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
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        switch (activeMode)
        {
        case 0:
            clockHour.fillSprite(BLACK);
            time.remove(5, 3);
            clockHour.setTextSize(3);
            clockHour.drawCentreString(time, 240, 0, 7); // TODO zmienić font
            clockHour.pushSprite(0, 88);
            break;
        case 1:
            clockHour.fillSprite(BLACK);
            clockHour.setTextSize(1);
            clockHour.drawCentreString(time, 240, 34, 8);
            clockHour.pushSprite(0, 88);

            sprintf(buffer, "%s, %s %s %s", date.substring(0, date.indexOf(".")), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth(), date.substring(date.indexOf(".") + 7, date.indexOf(".") + 11));
            clockDate.fillSprite(BLACK);
            clockDate.setTextSize(1);
            clockDate.drawCentreString(String(buffer), 240, 0, 4);
            clockDate.pushSprite(0, 232);
            break;
        case 2:
            clockHour.fillSprite(BLACK);
            clockHour.setTextSize(3);
            time.remove(5, 3);
            clockHour.drawCentreString(time, 240, 16, 6);
            clockHour.pushSprite(0, 88);

            sprintf(buffer, "%s, %s %s", date.substring(0, 3), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth());
            clockDate.fillSprite(BLACK);
            clockDate.setTextSize(1);
            clockDate.drawString(String(buffer), 32, 0, 4);

            sprintf(buffer, "°C %2.0f", round(bme.readTemperature()));
            clockDate.drawString(String(buffer), 306, 0, 4); // TODO zmienic font

            sprintf(buffer, "%H %2.0f", round(bme.readHumidity()));
            clockDate.drawString(String(buffer), 381, 0, 4); // TODO zmienic font
            clockDate.pushSprite(0, 248);
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
    activeScreenElement = &ClockScreen;

    if (activeMode == 0)
        statusBarWiFiActive = false;
    drawWiFiStatus();

    statusBarClockActive = false;
    drawStatusBarClock();

    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        clockBackground.createSprite(480, 320);
        clockBackground.fillSprite(BLACK);
        clockBackground.pushSprite(0, 40);

        clockHour.createSprite(480, 144);
        clockDate.createSprite(480, 36);

        clockHour.setTextColor(WHITE);
        clockDate.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    xTaskCreate(updateClock, "updateClock", 20048, NULL, 3, &updateScreenElement_t); // TODO pamięć
}
