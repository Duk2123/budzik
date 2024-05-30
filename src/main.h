#ifndef MAIN_H
#define MAIN_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

void alarmPopUp(void *params);
extern TFT_eSPI tft;

String convertSpecialLetters(String text);

extern int prevBrightness;
extern int brightness;
void setBrightness(int value);

int degToDirection(int degrees);

uint16_t hexToColor(String hex);

extern TaskHandle_t updateScreenElement_t;
extern SemaphoreHandle_t tftMutex;
extern TaskHandle_t handlePopup_t;

void alarmPopUp(void *params);

class UserAlarm;
void saveVectorToFile(const char *filename, std::vector<UserAlarm> &data);

extern std::vector<UserAlarm> alarms;
class UserAlarm
{
private:
    bool enabled;
    int alarmHours;   // 0-24
    int alarmMinutes; // 0-60
    int alarmDay;     // 0-6
    bool isRepeating = false;
    std::array<bool, 7> repeatOnDayOfWeek; // Index 0 - Sunday ... 6 - Saturday

public:
    UserAlarm(String time = "00:00", std::array<bool, 7> dayOfRepeat = {false, false, false, false, false, false, false})
    {
        // TODO add an time validation check

        DateTime now = rtc.now();
        enabled = true;
        alarmHours = time.substring(0, 2).toInt();
        alarmMinutes = time.substring(3, 5).toInt();

        if (std::any_of(dayOfRepeat.begin(), dayOfRepeat.end(), [](bool x)
                        { return x; }))
            isRepeating = true;
        repeatOnDayOfWeek = dayOfRepeat;

        int today = now.dayOfTheWeek();
        if (isRepeating)
        {
            if (repeatOnDayOfWeek[today] && (alarmHours * 60 + alarmMinutes > now.hour() * 60 + now.minute()))
            {
                alarmDay = today;
            }
            else
            {
                for (int i = today + 1; i <= today + 7; i++)
                {
                    if (repeatOnDayOfWeek[i % 7])
                    {
                        alarmDay = i % 7;
                        break;
                    }
                };
            }
        }
        else
        {
            if (alarmHours * 60 + alarmMinutes > now.hour() * 60 + now.minute())
                alarmDay = today;
            else
                alarmDay = (today + 1) % 7;
        }
    }

    /// @brief enables/disables alarm
    /// @return current state of alarm
    bool toggleAlarm()
    {
        if (enabled)
            enabled = false;
        else
            enabled = true;

        return enabled;
    }

    bool isActive()
    {
        return enabled;
    }

    /// @brief Returns time until next alarm activation
    /// @return String "x day/s x hour/s x minute/s"
    String timeUntilAlarm()
    {
        if (enabled)
        {
            DateTime now = rtc.now();
            uint32_t nowUnix = now.unixtime();
            String result;
            char buffer[32];
            long int temp = (nowUnix + alarmHours * 3600 + alarmMinutes * 60 + (alarmDay - now.dayOfTheWeek()) * 86400 - now.minute() * 60 - now.hour() * 3600) - nowUnix;
            temp = temp < 0 ? 604800 + temp : temp;

            int days = temp / 86400;
            temp -= days * 24 * 60 * 60;
            int hours = temp / 3600;
            temp -= hours * 60 * 60;
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

    /// @brief Activates alarm
    void activateAlarm()
    {
        xTaskCreate(alarmPopUp, "alarmPopUp", 5012, NULL, 4, &handlePopup_t);
        Serial.println("Activating alarm");
        if (isRepeating)
        {
            for (int i = alarmDay + 1; i <= i + 7; i++)
            {
                if (repeatOnDayOfWeek[i % 7])
                {
                    alarmDay = i % 7;
                    break;
                }
            }
        }
        else
            enabled = false;
        saveVectorToFile("/alarms", alarms);
    }

    /// @brief Returns time of the alarm
    /// @return  String "hh:mm"
    String getTime()
    {
        char buffer[32];
        sprintf(buffer, "%02d:%02d", alarmHours, alarmMinutes);
        return String(buffer);
    }

    /// @brief Returns occurrences the alarm
    /// @return  String
    String getOccurrences()
    {
        if (isRepeating)
        {
            if (std::all_of(repeatOnDayOfWeek.begin(), repeatOnDayOfWeek.end(), [](bool x)
                            { return x; }))
                return "Daily";
            else if (std::all_of(repeatOnDayOfWeek.begin() + 1, repeatOnDayOfWeek.end() - 1, [](bool x)
                                 { return x; }) &&
                     repeatOnDayOfWeek[0] == false && repeatOnDayOfWeek[6] == false)
                return "Mon to Fri";
            else
            {
                String temp;
                for (int i = 0; i < 7; i++)
                {
                    if (repeatOnDayOfWeek[i])
                        temp += String(daysOfTheWeek[i]).substring(0, 2) + " ";
                }
                return temp;
            }
        }
        return "Once";
    }

    void serialize(File &file)
    {
        file.write(reinterpret_cast<uint8_t *>(&enabled), sizeof(enabled));
        file.write(reinterpret_cast<uint8_t *>(&alarmHours), sizeof(alarmHours));
        file.write(reinterpret_cast<uint8_t *>(&alarmMinutes), sizeof(alarmMinutes));
        file.write(reinterpret_cast<uint8_t *>(&alarmDay), sizeof(alarmDay));
        file.write(reinterpret_cast<uint8_t *>(&isRepeating), sizeof(isRepeating));
        file.write(reinterpret_cast<uint8_t *>(&repeatOnDayOfWeek), sizeof(repeatOnDayOfWeek));
    }
    void deserialize(File &file)
    {
        file.read(reinterpret_cast<uint8_t *>(&enabled), sizeof(enabled));
        file.read(reinterpret_cast<uint8_t *>(&alarmHours), sizeof(alarmHours));
        file.read(reinterpret_cast<uint8_t *>(&alarmMinutes), sizeof(alarmMinutes));
        file.read(reinterpret_cast<uint8_t *>(&alarmDay), sizeof(alarmDay));
        file.read(reinterpret_cast<uint8_t *>(&isRepeating), sizeof(isRepeating));
        file.read(reinterpret_cast<uint8_t *>(&repeatOnDayOfWeek), sizeof(repeatOnDayOfWeek));
    }
};

extern TaskHandle_t alarmAudio_t;
void alarmAudio(void *params);

extern TaskHandle_t alarmInterrupt_t;

#endif