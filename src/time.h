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
void syncRtcToNtp();

#endif