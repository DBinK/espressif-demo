#include <Arduino.h>
#include <ESPMDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include "LevelUri.h"

const char *ssid = "test0";
const char *password = "12345687";
const char *board = "esp32doit-devkit-v11";
WebServer server(80);

/***
 * 50KB缓存
 * */
#define buff_size  (50 * 1024)
uint8_t buff[buff_size];

void handleNotFound() {
    String message = "File Not Foundnn";
    message += "URI: ";
    message += server.uri();
    message += "nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "nArguments: ";
    message += server.args();
    message += "n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "n";
    }

    server.send(404, "text/plain", message);
}
String getContentType(String filename) {
    if (server.hasArg("download")) {
        return "application/octet-stream";
    } else if (filename.endsWith(".htm")) {
        return "text/html";
    } else if (filename.endsWith(".html")) {
        return "text/html";
    } else if (filename.endsWith(".css")) {
        return "text/css";
    } else if (filename.endsWith(".js")) {
        return "application/javascript";
    } else if (filename.endsWith(".png")) {
        return "image/png";
    } else if (filename.endsWith(".gif")) {
        return "image/gif";
    } else if (filename.endsWith(".jpg")) {
        return "image/jpeg";
    } else if (filename.endsWith(".ico")) {
        return "image/x-icon";
    } else if (filename.endsWith(".xml")) {
        return "text/xml";
    } else if (filename.endsWith(".pdf")) {
        return "application/x-pdf";
    } else if (filename.endsWith(".zip")) {
        return "application/x-zip";
    } else if (filename.endsWith(".gz")) {
        return "application/x-gzip";
    } else if (filename.endsWith(".svg")){
        return "image/svg+xml";
    }
    return "text/plain";
}

inline void handleFile(const char *path) {
    File file = SPIFFS.open(path, FILE_READ);
    if (!file) {
        log_i("There was an error opening the file for writing");
        handleNotFound();
        return;
    }
    size_t length = file.size();

    String header =
            "HTTP/1.1 200 OK\n"
            "Content-Type: {0}\n"
            "Content-Length: {1}\n"
            "Connection: close\n\n";
    header.replace("{0}", getContentType(String(path)));
    header.replace("{1}", String(length));
    auto client = server.client();
    client.write(header.c_str());
    client.flush();
    int count = length / buff_size;
    for (int i = 0; i < count; i++) {
        file.read(buff, buff_size);
        client.write(buff, buff_size);
        client.flush();
    }
    int lastLen = length % buff_size;
    if (lastLen) {
        file.read(buff, lastLen);
        client.write(buff, lastLen);
        client.flush();
    }
    file.close();
}

void handleRoot() {
    handleFile("/index.html");
}

void favicon() {
    handleFile("/favicon.ico");
}

void handleAssets() {
    const String &string = server.uri();
    handleFile(string.c_str());
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

    server.on("/favicon.ico", favicon);
    server.on("/board", []() { server.send(200, "text/plain", board); });

    server.on(LevelUri("/assets"), handleAssets);
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
