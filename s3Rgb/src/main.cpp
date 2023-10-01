#include <Arduino.h>
#include <ESPMDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include "index_html.h"
#include <uri/UriBraces.h>

const char *ssid = WIFI_SSID;
const char *password = PASSWORD;
WebServer server(80);

void handleRoot() {

    server.sendHeader("Content-Encoding", "gzip");
    server.send_P(200, "text/html", (const char *) index_gz, index_len);
}

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

void setRgb() {
    String rgb = server.pathArg(0);
    if (!rgb) {
        return;
    }
    int rgbInt = rgb.toInt();
    log_i("rgb:%06X", rgbInt);
    neopixelWrite(RGB_BUILTIN, 0xff & (rgbInt >> 16), 0xff & (rgbInt >> 8), 0xff & rgbInt);
    server.send(200, "text/plain", "ok");
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

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
    if (MDNS.begin(ESP32_SEVER_HOST_NAME)) {
        Serial.println("MDNS responder started");
        log_i("MDNS responder started");
    }

    server.on("/", handleRoot);
    server.on(UriBraces("/rgb/{}"), setRgb);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
    const IPAddress &address = WiFi.localIP();
    Serial.print("http://");
    Serial.println(address.toString());
    Serial.print("http://");
    Serial.println(ESP32_SEVER_HOST_NAME);
    log_i("http://"
                  ESP32_SEVER_HOST_NAME
    );

    log_i("http://%u.%u.%u.%u", address[0], address[1], address[2], address[3]);
}

void loop() {
    server.handleClient();
    delay(2);
}