#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS1302.h>

Adafruit_AHTX0 aht;
Adafruit_SSD1306 display(128, 64, &Wire, -1);
ThreeWire myWire(4, 5, 2); // DAT/IO, CLK/SCLK, RST/CE
RtcDS1302<ThreeWire> Rtc(myWire);

void checkRTC()
{
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid())
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
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
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time! (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
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

TaskHandle_t Task1;
void touchThread(void *parameter)
{
  for (;;)
  {
    touch_value_t valueT = touchRead(T3);

    if (valueT < 30)
    {
      digitalWrite(18, HIGH);
    }
    else
    {
      digitalWrite(18, LOW);
    }

    delay(1);
  }
}

void setup()
{
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (1)
    delay(10);
  }

  display.display();

  Rtc.Begin();
  checkRTC();

  if (!aht.begin())
  {
    Serial.println("Could not find AHT25 sensor!");
    while (1)
      delay(10);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  display.print(String(temp.temperature, 1));
  // https://en.wikipedia.org/wiki/Code_page_437#Character_set
  display.write(248);
  display.print("C");

  String humidityValue = String(humidity.relative_humidity, 1) + "%";
  display.setCursor(display.width() - (humidityValue.length() * 6), 0);
  display.println(humidityValue);

  display.drawLine(0, 15, display.width()-1, 15, SSD1306_WHITE);
  display.setCursor(0, 12);
  display.setTextSize(2);

  RtcDateTime now = Rtc.GetDateTime();

  const char* hour = getHour(now);
  const char* date = getDate(now);

  int16_t x, y;
  uint16_t w, h;
  display.getTextBounds(hour, 0, 0, &x, &y, &w, &h);
  display.setCursor((display.width() - w) / 2, 25);
  display.println(hour);

  display.setTextSize(1);
  display.getTextBounds(date, 0, 0, &x, &y, &w, &h);
  display.setCursor((display.width() - w) / 2, 45);
  display.println(date);

  display.display();
  delay(1000);
}
