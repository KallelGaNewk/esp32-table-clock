# ESP32 Table Clock with Temperature and Humidity

## Components used

- ESP32
- AHT25 (or any other AHTxx sensor)
- DS1302 RTC + 3V CR2032 battery
- SD1306 OLED display (128x64)
- 1x 1kΩ resistor (in AHT25 ground)

if your ESP32 devkit doesn't have internal pull-ups, you will need to add external pull-ups for the I2C lines (SDA and SCL).
- 2x 10kΩ resistors (for AHT25 I2C pull-ups)

# Web server routes
- `GET /` - Displays the HTML form for setting WiFi.
- `POST /save` - Saves the WiFi credentials from the form and try connecting.
- `GET /status` - Returns the current temperature and humidity in JSON format.
