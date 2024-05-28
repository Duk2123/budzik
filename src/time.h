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

extern char daysOfTheWeek[7][12];

#endif