#include "wifi_manager.h"
#include "globals.h"

#include <WiFi.h>
#include <WebServer.h>
#include <WString.h>

WebServer server(80);
String ipAddress = "";
bool APisEnabled = false;

TaskHandle_t WiFiManagerHandler;

String inputSSID;
String inputPass;

const char *htmlForm PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head><title>WiFi Configuration</title></head>
<body>
  <h2>ESP32 WiFi</h2>
  <form action="/save" method="POST">
    SSID: <input type="text" name="ssid"><br>
    Senha: <input type="password" name="pass"><br>
    <input type="submit" value="Save">
  </form>
</body>
</html>
)rawliteral";

const char *htmlSuccess PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head><title>Success!</title></head>
<body>
  <h2>Connected successfully!</h2>
  <p>IP: %s</p>
</body>
</html>
)rawliteral";

const char *htmlFail PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head><title>Failure</title></head>
<body>
  <h2>Failed to connect.</h2>
  <p>Check SSID/Password and try again.</p>
</body>
</html>
)rawliteral";

void setupWiFi(bool server);

bool tryConnecting(String ssid, String pass)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (millis() - start < 10000)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            prefs.begin("wifiCreds", false);
            prefs.putString("ssid", ssid);
            prefs.putString("pass", pass);
            prefs.end();

            ipAddress = WiFi.localIP().toString();
            APisEnabled = false;
            return true;
        }
        delay(500);
    }
    return false;
}

void handleSave()
{
    inputSSID = server.arg("ssid");
    inputPass = server.arg("pass");

    if (inputSSID.length() == 0)
    {
        server.send(400, "text/plain", "Blank SSID");
        return;
    }

    if (inputPass.length() == 0)
    {
        server.send(400, "text/plain", "Blank Password");
        return;
    }

    if (tryConnecting(inputSSID, inputPass))
    {
        char buf[200];
        snprintf(buf, sizeof(buf), htmlSuccess, WiFi.localIP().toString().c_str());
        server.send(200, "text/html", buf);
    }
    else
    {
        server.send(200, "text/html", htmlFail);
        setupWiFi(false);
    }
}

void handleStatus()
{
    String response = "{";
    response += "\"temperature\":" + String(temp.temperature) + ",";
    response += "\"humidity\":" + String(humidity.relative_humidity);
    response += "}";
    server.send(200, "application/json", response);
}

void setupServer()
{
    server.on("/", HTTP_GET, []()
              { server.send(200, "text/html", htmlForm); });
    server.on("/save", HTTP_POST, handleSave);
    server.on("/status", HTTP_GET, handleStatus);
    server.begin();
}

void setupWiFi(bool server)
{
    prefs.begin("wifiCreds", true);
    String savedSSID = prefs.getString("ssid");
    String savedPass = prefs.getString("pass");
    prefs.end();

    if (savedSSID.length() > 0 && savedPass.length() > 0 && tryConnecting(savedSSID, savedPass))
    {
    }
    else
    {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
        WiFi.softAP("Table Clock");
        ipAddress = WiFi.softAPIP().toString();
        APisEnabled = true;
    }

    if (server)
    {
        setupServer();
    }
}

void WiFiManagerTask(void *parameter)
{
    for (;;)
    {
        server.handleClient();

        if (WiFi.status() != WL_CONNECTED && !APisEnabled)
        {
            setupWiFi(false);
        }

        delay(1);
    }
}
