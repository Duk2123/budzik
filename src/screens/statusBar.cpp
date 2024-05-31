#include <screens/screens.h>
#include <time.h>

TFT_eSprite statusBarBackground = TFT_eSprite(&tft);
TFT_eSprite statusBarWiFi = TFT_eSprite(&tft);
TFT_eSprite statusBarClock = TFT_eSprite(&tft);

TaskHandle_t statusBar_t;

bool statusBarWiFiActive = true;
bool changeToIpMode = false;

void drawWiFiStatus()
{

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        if (statusBarWiFiActive)
        {
            statusBarWiFi.fillSprite(BLACK);
            if (WiFi.status() == WL_CONNECTED)
            {

                statusBarWiFi.fillRect(0, 0, 32, 32, hexToColor("D9D9D9")); // TODO ikona
                statusBarWiFi.drawString(changeToIpMode ? WiFi.localIP().toString() : WiFi.SSID(), 40, 8, 2);
            }
            else
            {
                statusBarWiFi.fillRect(0, 0, 32, 32, hexToColor("5D5E5F"));
            }
            statusBarWiFi.pushSprite(8, 8);
        }
        else
        {
            statusBarWiFi.fillSprite(BLACK);
            statusBarWiFi.pushSprite(8, 8);
        }
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

bool statusBarClockActive = false;

void drawStatusBarClock()
{
    String time = getRtcTime();
    time.remove(5, 3);

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        if (statusBarClockActive)
        {
            statusBarClock.fillSprite(BLACK);
            statusBarClock.drawString(time, 0, 8, 2);
            statusBarClock.pushSprite(416, 8);
        }
        else
        {
            statusBarClock.fillSprite(BLACK);
            statusBarClock.pushSprite(416, 8);
        }
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void statusBar(void *params)
{
    String prevTime = "00:00";
    String time;

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        statusBarBackground.createSprite(464, 32);
        statusBarWiFi.createSprite(232, 32);

        statusBarBackground.fillSprite(BLACK);
        statusBarBackground.pushSprite(8, 8);

        statusBarWiFi.setTextColor(WHITE);

        statusBarClock.createSprite(48, 32);
        statusBarClock.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawWiFiStatus();

    for (;;)
    {
        if (statusBarWiFiActive)
        {
            drawWiFiStatus();
        }

        time = getRtcTime();
        time.remove(5, 3);
        if (statusBarClockActive && time != prevTime)
        {
            prevTime = time;
            drawStatusBarClock();
        }

        vTaskDelay(1000);
    }
}