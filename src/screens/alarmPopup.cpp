#include <main.h>
#include <screens/screens.h>
#include <touch.h>
#include "screens.h"

TFT_eSprite AlarmBackground = TFT_eSprite(&tft);
TFT_eSprite AlarmClock = TFT_eSprite(&tft);
TFT_eSprite AlarmDate = TFT_eSprite(&tft);
TFT_eSprite AlarmInfo = TFT_eSprite(&tft);

int snoozeCounter = 0;

void drawAlarmClock()
{
    String time = getRtcTime();
    String date = getRtcDate();
    time.remove(5, 3);

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmClock.fillSprite(BLACK);
        AlarmClock.drawCentreString(time, 240, 0, 6);
        AlarmClock.pushSprite(0, 48);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawAlarmDate()
{
    String date = getRtcDate();
    char buffer[50];

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmDate.fillSprite(BLACK);
        sprintf(buffer, "%s, %s %s %s", date.substring(0, date.indexOf(".")), date.substring(date.indexOf(".") + 1, date.indexOf(".") + 3), getMonth(), date.substring(date.indexOf(".") + 7, date.indexOf(".") + 11));
        AlarmDate.drawCentreString(String(buffer), 240, 0, 4);
        AlarmDate.pushSprite(0, 192);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawAlarmInfo()
{

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmInfo.fillSprite(BLACK);
        AlarmInfo.setTextSize(1);
        AlarmInfo.drawCentreString("swipe to disable", 240, 0, 2);
        AlarmInfo.setTextSize(2);
        AlarmInfo.drawCentreString("\\/", 240, 16, 4);
        AlarmInfo.pushSprite(0, 240);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

bool alarmEndCalled = false;
void alarmHandleSwipe()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        Serial.println("alarm swiped");
        detectTouchSuspendCounter = 4;
        delay(16);
        alarmEndCalled = true;
    }
}

bool snooze = false;
bool alarmSnoozeCalled = false;
void alarmHandlePress()
{
    if (!snooze && snoozeCounter < 4)
    {
        snoozeCounter++;
        detectTouchSuspendCounter = 4;
        delay(16);
        alarmSnoozeCalled = true;
    }
}

ScreenObject AlarmPopUp({{0, 0, 480, 320}}, {{0, 0, 480, 320}}, {},
                        {alarmHandlePress}, {alarmHandleSwipe}, {});

void alarmPopUp(void *params)
{

    vTaskSuspend(statusBar_t);
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskDelete(updateScreenElement_t);
    }

    vTaskSuspend(alarmInterrupt_t);
    vTaskSuspend(autoSyncRtc_t);

    ActiveScreenElement = &AlarmPopUp;

    vTaskDelay(64);
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmBackground.createSprite(480, 320);
        AlarmClock.createSprite(480, 112);
        AlarmDate.createSprite(480, 32);
        AlarmInfo.createSprite(480, 72);

        AlarmBackground.fillSprite(BLACK);
        AlarmBackground.pushSprite(0, 0);

        AlarmInfo.setTextColor(WHITE);

        AlarmClock.setTextColor(WHITE);
        AlarmClock.setTextSize(3);

        AlarmDate.setTextColor(WHITE);
    }
    vTaskDelay(32);
    xSemaphoreGive(tftMutex);

    snoozeCounter = 0;

    setBrightness(100);

    drawAlarmClock();
    drawAlarmDate();
    drawAlarmInfo();

    vTaskDelay(500);
    xTaskCreate(alarmAudio, "alarmAudio", 4096, NULL, 10, &alarmAudio_t);

    uint32_t snoozeTime;

    uint32_t startUnixTime = rtc.now().unixtime();

    String startTime = getRtcTime();
    startTime.remove(5, 3);

    String time;
    String prevTime = "00:00";

    while (alarmEndCalled == false)
    {
        time = getRtcTime();
        time.remove(5, 3);
        if (prevTime != time)
        {
            drawAlarmClock();
            drawAlarmDate();
        }
        if (alarmSnoozeCalled)
        {
            snoozeTime = rtc.now().unixtime();
            vTaskDelete(alarmAudio_t);
            snooze = true;
            alarmSnoozeCalled = false;
        }
        if (snooze && rtc.now().unixtime() - snoozeTime > 60)
        {
            xTaskCreate(alarmAudio, "alarmAudio", 4096, NULL, 10, &alarmAudio_t);
            snooze = false;
        }
        vTaskDelay(300);
    }
    alarmEndCalled = false;
    if (alarmAudio_t != NULL && eTaskGetState(alarmAudio_t) != 4)
        vTaskDelete(alarmAudio_t);

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmBackground.deleteSprite();
        AlarmClock.deleteSprite();
        AlarmDate.deleteSprite();
        AlarmInfo.deleteSprite();
    }
    vTaskDelay(8);
    xSemaphoreGive(tftMutex);

    Serial.println("Alarm end");

    vTaskResume(statusBar_t);
    drawWiFiStatus();
    drawStatusBarClock();

    vTaskResume(alarmInterrupt_t);
    vTaskResume(autoSyncRtc_t);

    setBrightness(prevBrightness);

    clockScreen();

    char buffer[64];

    File file = SD.open("/logs/alarms.csv", FILE_APPEND);
    sprintf(buffer, "%d,%s,%d,%d", startUnixTime, startTime, rtc.now().unixtime() - startUnixTime, snoozeCounter);
    file.println(buffer);
    file.close();

    vTaskDelete(NULL);
}
