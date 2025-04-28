#include <RtcDS1302.h>
#include "time_helper.h"

ThreeWire myWire(4, 5, 2); // DAT/IO, CLK/SCLK, RST/CE
RtcDS1302<ThreeWire> Rtc(myWire);
bool needsBatteryReplacement = false;

RtcDateTime getNTPTime()
{
    configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10000))
    {
        Serial.println("Failed to obtain time from NTP server");
        return RtcDateTime(__DATE__, __TIME__);
    }

    time_t now;
    time(&now);
    now += TIMEZONE_OFFSET * 3600; // Adjust for timezone offset
    gmtime_r(&now, &timeinfo);

    Serial.println("NTP time obtained successfully");
    Serial.printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
                  timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    return RtcDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                       timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void checkRTC()
{
    RtcDateTime ntpTime = getNTPTime();

    if (!Rtc.IsDateTimeValid()) {
        needsBatteryReplacement = true;
    }
    if (Rtc.GetIsWriteProtected()) {
        Rtc.SetIsWriteProtected(false);
    }
    if (!Rtc.GetIsRunning()) {
        Rtc.SetIsRunning(true);
    }

    if (ntpTime == RtcDateTime(__DATE__, __TIME__)) return; // ntpTime is not valid

    RtcDateTime now = Rtc.GetDateTime();
    int32_t timeDifference = now.TotalSeconds() - ntpTime.TotalSeconds();

    if (abs(timeDifference) > TIME_THRESHOLD)
    {
        Serial.printf("RTC time differs by %d seconds from NTP time! (Updating DateTime)\n", timeDifference);
        Rtc.SetDateTime(ntpTime);
    }
}

const char *getDate(const RtcDateTime &dt)
{
    static char buf[11];
    snprintf(buf,
             sizeof(buf),
             "%02u/%02u/%04u",
             dt.Day(),
             dt.Month(),
             dt.Year());
    return buf;
}

const char *getHour(const RtcDateTime &dt)
{
    static char buf[9];
    snprintf(buf,
             sizeof(buf),
             "%02u:%02u:%02u",
             dt.Hour(),
             dt.Minute(),
             dt.Second());
    return buf;
}
