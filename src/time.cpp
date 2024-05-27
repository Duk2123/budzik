#include <time.h>
#include <network.h>
#include <WiFi.h>
#include <screens/screens.h>

TwoWire I2C_DS3231 = TwoWire(0);
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char months[12][20] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

/** @brief RTC setup */
void setupRtc()
{
    I2C_DS3231.begin(DS3231_SDA, DS3231_SCL);
    if (!rtc.begin(&I2C_DS3231))
    {
        Serial.println("RTC setup failed"); // TODO obsługa błędów
        return;
    }

    Serial.println("RTC setup done");
}

/**
 * @brief Returns a String with current RTC time
 * @return String "hh:mm:ss"
 */
String getRtcTime()
{
    DateTime now = rtc.now();
    char buffer[16];
    sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    return String(buffer);
}

/**
 * @brief Returns a String with current RTC date
 * @return String "DayOfWeek.DD.MM.YYYY"
 */
String getRtcDate()
{
    DateTime now = rtc.now();
    char buffer[32];
    sprintf(buffer, "%s.%02d.%02d.%04d", daysOfTheWeek[now.dayOfTheWeek()], now.day(), now.month(), now.year());
    return String(buffer);
}

//
/**
 * @brief Returns a String with the name of the month
 * @return String
 */
String getMonth()
{
    DateTime now = rtc.now();
    return String(months[now.month() - 1]);
}

/** @brief Synchronizes RTC to time from NTP server */
void syncRtcToNtp()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            uint64_t epoch = timeClient.getEpochTime();
            DateTime dt(epoch);
            rtc.adjust(dt);
            Serial.println("Time synced");
        }
        delay(16);
        xSemaphoreGive(tftMutex);
    }
}

TaskHandle_t autoSyncRtc_t;
/** @brief Task for automatic RTC synchronization */
void autoSyncRtc(void *params)
{
    vTaskDelay(60000);
    uint64_t epoch;
    DateTime dt;

    for (;;)
    {
        if (WiFi.status() == WL_CONNECTED && timeClient.isTimeSet())
        {
            syncRtcToNtp();
        }
        vTaskDelay(1000 * 60 * 60 * 24);
    }
}
