#pragma once

#include <WiFi.h>
#include <WebServer.h>

extern TaskHandle_t WiFiManagerHandler;
extern WebServer server;

extern String ipAddress;
extern bool APisEnabled;

void setupWiFi(bool server = true);
void WiFiManagerTask(void *pvParameters);
