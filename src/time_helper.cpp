#include <RtcDS1302.h>

ThreeWire myWire(4, 5, 2); // DAT/IO, CLK/SCLK, RST/CE
RtcDS1302<ThreeWire> Rtc(myWire);

RtcDateTime getNTPTime() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10000))
    {
        return RtcDateTime(__DATE__, __TIME__);
    }

    return RtcDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                       timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void checkRTC() {
    RtcDateTime ntpTime = getNTPTime();

    if (!Rtc.IsDateTimeValid())
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(ntpTime);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < ntpTime)
    {
        Serial.println("RTC is older than compile time! (Updating DateTime)");
        Rtc.SetDateTime(ntpTime);
    }
    else if (now > ntpTime)
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == ntpTime)
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
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
