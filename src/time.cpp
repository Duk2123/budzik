#include <time.h>
#include <network.h>
#include <WiFi.h>

TwoWire I2C_DS3231 = TwoWire(0);
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char months[12][20] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

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

// Returns a String "hh:mm:ss" with current RTC time
String getRtcTime()
{
    DateTime now = rtc.now();
    char buffer[16];
    sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    return String(buffer);
}

// Returns a String "DayOfWeek.DD.MM.YYYY" with current RTC date
String getRtcDate()
{
    DateTime now = rtc.now();
    char buffer[32];
    sprintf(buffer, "%s.%02d.%02d.%04d", daysOfTheWeek[now.dayOfTheWeek()], now.day(), now.month(), now.year());
    return String(buffer);
}

// Returns a String with the name of the month
String getMonth()
{
    DateTime now = rtc.now();
    return String(months[now.month() - 1]);
}

void syncRtcToNtp()
{ // TODO zrobić taska z automatyczną synchronizacją jeśli różnica między ntp a rtc > 1 min
    if (WiFi.status() == WL_CONNECTED)
    {
        uint64_t epoch = timeClient.getEpochTime();
        DateTime dt(epoch);
        rtc.adjust(dt);
        Serial.println("Time synced");
    }
}

TaskHandle_t autoSyncRtc_t;
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