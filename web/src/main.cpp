#include <Arduino.h>
#include <ESPMDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"

const char* ssid = "test0";
const char* password = "12345687";
const char* board = "esp32doit-devkit-v11";
WebServer server(80);

/***
 * 50KB缓存
 * */
uint8_t buff[50 * 1024];

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}
void handleRoot() {
    File file = SPIFFS.open("/index.html", FILE_READ);
    if (!file) {
        log_i("There was an error opening the file for writing");
        handleNotFound();
        return;
    }
    size_t length = file.size();
    file.read(buff, length);
    server.send_P(200, "text/html", (const char*)buff, length);
}

void setup() {
    Serial.begin(115200);
    SPIFFS.begin();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);


    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }
    server.on("/", handleRoot);
    server.on("/board", []() { server.send(200, "text/plain", board); });

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
    Serial.print("http://");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();
    delay(2);  //允许cpu切换，使得其它任务可以调度
}
