//
// Created by immor on 2024/6/9.
//

#include "WebServer.h"
#include "index.h"
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

/* A simple example that demonstrates using websocket echo server
 */
static const char *TAG = "ws_echo_server";
SemaphoreHandle_t xMutex{nullptr};

static QueueHandle_t ws_msg_group = nullptr;

#define buff_size (10 * 1024)
static uint8_t buffer[buff_size] = {};

struct ws_msg {
    const char *msg;
    uint len;
};
/*
 * Structure holding server handle
 * and internal socket fd in order
 * to use out of request send
 */
struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

/*
 * async send function, which we put into the httpd work queue
 */
static void ws_async_send(void *arg) {
    static const char *data = "Async data";
    auto *resp_arg = static_cast<async_resp_arg *>(arg);
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *) data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req) {
    auto *resp_arg = static_cast<async_resp_arg *>(malloc(sizeof(struct async_resp_arg)));
    if (resp_arg == NULL) {
        return ESP_ERR_NO_MEM;
    }
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    esp_err_t ret = httpd_queue_work(handle, ws_async_send, resp_arg);
    if (ret != ESP_OK) {
        free(resp_arg);
    }
    return ret;
}

String getContentType(const String &filename) {
    if (filename.endsWith(".htm")) {
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
    } else if (filename.endsWith(".svg")) {
        return "image/svg+xml";
    }
    return "text/plain";
}

/*
 * This handler echos back the received ws data
 * and triggers an async send if certain message received
 */
esp_err_t echo_handler(httpd_req_t *req) {
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = nullptr;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = static_cast<uint8_t *>(calloc(1, ws_pkt.len + 1));
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s , final:%d  fragmented:%d", ws_pkt.payload, ws_pkt.final,
                 ws_pkt.fragmented);
    }
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);


    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }
    free(buf);
    ws_msg wsMsg{};
    ws_pkt.payload = new uint8_t[128];
    for (;;) {
        if (xQueueReceive(ws_msg_group, &wsMsg, portMAX_DELAY)) {
            memcpy(ws_pkt.payload, (void *) wsMsg.msg, wsMsg.len);
            ws_pkt.len = wsMsg.len;
            ESP_LOGI(TAG, "send frame, %*s", wsMsg.len, wsMsg.msg);
            ret = httpd_ws_send_frame(req, &ws_pkt);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
                break;
            }
        }
    }
    return ret;
}

esp_err_t file_handler(httpd_req_t *req) {
    const char *uri = req->uri;
    String path = String(uri);
    if (path == "/") {
        path = "/index.html";
    }
    File file = SPIFFS.open(path + ".gz", FILE_READ);
    if (!file) {
        ESP_LOGE(TAG, "file not found %s", uri);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    size_t length = file.size();
    String header =
            "HTTP/1.1 200 OK\n"
            "Content-Type: {0}\n"
            "Content-Length: {1}\n"
            "Content-Encoding: gzip\n"
            "Connection: close\n\n";
    header.replace("{0}", getContentType(path));
    header.replace("{1}", String(length));
    httpd_send(req, header.c_str(), header.length());
    size_t count = length / buff_size;
    for (int i = 0; i < count; i++) {
        file.read(buffer, buff_size);
        httpd_send(req, reinterpret_cast<const char *>(buffer), buff_size);
    }
    size_t lastLen = length % buff_size;
    if (lastLen) {
        file.read(buffer, lastLen);
        httpd_send(req, reinterpret_cast<const char *>(buffer), lastLen);
    }
    file.close();
    return ESP_OK;
}

static const httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler = echo_handler,
        .user_ctx   = nullptr,
        .is_websocket = true
};

static const httpd_uri_t root = {
        .uri        = "/",
        .method     = HTTP_GET,
        .handler = file_handler,
        .user_ctx   = nullptr,
        .is_websocket = false
};

httpd_handle_t start_webserver() {
    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Registering the ws handler
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &ws);
        httpd_register_uri_handler(server, &root);
        for (const auto &item: webSrc) {
            httpd_uri_t fileUri = {
                    .uri        = item,
                    .method     = HTTP_GET,
                    .handler = file_handler,
                    .user_ctx   = nullptr,
                    .is_websocket = false
            };
            httpd_register_uri_handler(server, &fileUri);
        }

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return nullptr;
}


void example_connect(const char *ssid, const char *password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    // wait for WiFi connection
    ESP_LOGI(TAG, "Waiting for WiFi to connect...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        printf(".");
    }
    const IPAddress &address = WiFi.localIP();
    ESP_LOGI(TAG, "connected, localIP: %s", address.toString().c_str());
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data) {
    auto *server = (httpd_handle_t *) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


namespace WebServer {

    void start(const char *ssid, const char *password) {
        xMutex = xSemaphoreCreateMutex(); // 创建互斥锁
        if (xMutex == nullptr) {
            ESP_LOGI(TAG, "xSemaphoreCreateMutex failed");
            abort();
        }
        static httpd_handle_t server = nullptr;
        ws_msg_group = xQueueCreate(10, sizeof(ws_msg));

        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        example_connect(ssid, password);
        if (MDNS.begin(ESP32_SEVER_HOST_NAME)) {
            ESP_LOGI(TAG, "MDNS responder started");
        }
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
        server = start_webserver();


    }

    void send(const String &gyroInfo) {
        ws_msg wsMsg{
                const_cast<char *>(gyroInfo.c_str()), gyroInfo.length()
        };
        xQueueSend(ws_msg_group, &wsMsg, 300);
    }
} // namespace WebServer

static esp_err_t stop_webserver(httpd_handle_t server) {
    // Stop the httpd server
    return httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    auto *server = (httpd_handle_t *) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}
