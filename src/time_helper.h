#pragma once
#include <RtcDS1302.h>

extern ThreeWire myWire;
extern RtcDS1302<ThreeWire> Rtc;

void checkRTC();
const char *getDate(const RtcDateTime &dt);
const char *getHour(const RtcDateTime &dt);
