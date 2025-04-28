#pragma once
#include <RtcDS1302.h>

extern ThreeWire myWire;
extern RtcDS1302<ThreeWire> Rtc;
extern bool needsBatteryReplacement;

void checkRTC();
const char *getDate(const RtcDateTime &dt);
const char *getHour(const RtcDateTime &dt);

#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.nist.gov"
#define TIMEZONE_OFFSET -3 // UTC
#define TIME_THRESHOLD 60 // seconds
