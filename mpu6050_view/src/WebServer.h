//
// Created by immor on 2024/6/9.
//

#ifndef MPU6050_VIEW_WEBSERVER_H
#define MPU6050_VIEW_WEBSERVER_H

#include <Arduino.h>
#include <esp_http_server.h>

namespace WebServer {
    void start(const char *ssid, const char *password);

    void send(const String &gyroInfo);
} // namespace WebServer


#endif //MPU6050_VIEW_WEBSERVER_H
