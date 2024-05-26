#ifndef TIME_H
#define TIME_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

#include "RTClib.h"
#define DS3231_SDA 1
#define DS3231_SCL 2

extern RTC_DS3231 rtc;
void setupRtc();
String getRtcTime();
String getRtcDate();
String getMonth();
void syncRtcToNtp();

extern TaskHandle_t autoSyncRtc_t;
void autoSyncRtc(void *params);

class Alarm
{
private:
    bool enabled;
    int alarmHours;   // 0-24
    int alarmMinutes; // 0-60
    int alarmDay;     // 0-6
    bool isRepeating;
    std::array<bool, 7> repeatOnDayOfWeek; // Index 0 - Sunday ... 6 - Saturday

public:
    Alarm(String time, bool repeats, std::array<bool, 7> dayOfRepeat = {false, false, false, false, false, false, false})
    {
        enabled = true;
        alarmHours = time.substring(0, 1).toInt();
        alarmMinutes = time.substring(3, 4).toInt();
        isRepeating = repeats;
        repeatOnDayOfWeek = dayOfRepeat;
    }

    /// @brief enables/disables alarm
    /// @return current state of alarm
    bool toggleAlarm()
    {
        if (enabled)
        {
            enabled = false;
        }
        else
        {
            enabled = true;
        }
        return enabled;
    }

    String timeUntilAlarm()
    {
        if (enabled)
        {
            DateTime now = rtc.now();
            uint32_t nowUnix = now.unixtime();
            String result;
            char buffer[32];
            unsigned int temp = (nowUnix - alarmHours * 3600 + alarmMinutes * 60 + (7 - abs(alarmDay - now.dayOfTheWeek())) * 86400 - now.minute() * 60 - now.hour() * 3600) - nowUnix;

            int days = temp / 86400;
            temp -= days * 24;
            int hours = temp / 3600;
            temp -= hours * 60;
            int minutes = temp / 60;

            if (days > 0)
            {
                sprintf(buffer, "%d %s ", days, days == 1 ? "day" : "days");
                result += String(buffer);
            }
            if (hours > 0 || days > 0)
            {
                sprintf(buffer, "%d %s ", hours, hours == 1 ? "hour" : "hours");
                result += String(buffer);
            }

            sprintf(buffer, "%d %s", minutes, minutes == 1 ? "minute" : "minutes");
            result += String(buffer);

            return result;
        }
        else
            return "";
    }

    void activateAlarm()
    { // if (now.hour() == alarmHours && now.minute() == alarmMinutes)
        if (isRepeating)
        {
        }
        else
            enabled = false;
    }
};

#endif