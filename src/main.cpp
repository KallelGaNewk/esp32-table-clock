#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SSD1306.h>

#include "globals.h"
#include "time_helper.h"
#include "wifi_manager.h"

Adafruit_AHTX0 aht;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

sensors_event_t humidity, temp;
Preferences prefs;

void setup()
{
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (1)
      delay(10);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.setCursor(0, 0);

  display.setTextSize(2);
  display.println(F("Table Clock"));

  display.setTextSize(1);
  display.println(F("Rtc setup..."));
  display.display();

  Rtc.Begin();
  checkRTC();

  display.println(F("Sensor setup..."));
  display.display();

  if (!aht.begin())
  {
    Serial.println("Could not find AHT25 sensor!");
    while (1)
      delay(10);
  }

  display.println(F("WiFi setup..."));
  display.display();

  setupWiFi();

  display.clearDisplay();

  xTaskCreatePinnedToCore(
      WiFiManagerTask,     /* Task function. */
      "WiFiManager",       /* name of task. */
      10000,               /* Stack size of task */
      NULL,                /* parameter of the task */
      1,                   /* priority of the task */
      &WiFiManagerHandler, /* Task handle to keep track of created task */
      0);                  /* pin task to core 0 */
}

void loop()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(2, 2);

  aht.getEvent(&humidity, &temp);

  display.print(String(temp.temperature, 1));
  // https://en.wikipedia.org/wiki/Code_page_437#Character_set
  display.write(248);
  display.print("C");

  String humidityValue = String(humidity.relative_humidity, 1) + "%";
  display.setCursor(display.width() - (humidityValue.length() * 6), 0);
  display.println(humidityValue);

  display.drawLine(0, 10, display.width() - 1, 10, SSD1306_WHITE);
  display.setCursor(0, 12);
  display.setTextSize(2);

  RtcDateTime now = Rtc.GetDateTime();

  const char *hour = getHour(now);
  const char *date = getDate(now);

  int16_t x, y;
  uint16_t w, h;
  display.getTextBounds(hour, 0, 0, &x, &y, &w, &h);
  display.setCursor((display.width() - w) / 2, 20);
  display.println(hour);

  display.setTextSize(1);
  display.getTextBounds(date, 0, 0, &x, &y, &w, &h);
  display.setCursor((display.width() - w) / 2, 40);
  display.println(date);

  String ip = "IP: " + ipAddress;
  display.getTextBounds(ip, 0, 0, &x, &y, &w, &h);
  display.setCursor((display.width() - w) / 2, 56);
  display.println(ip);

  display.display();
  delay(1000);
}
