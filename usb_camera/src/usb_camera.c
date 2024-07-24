/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usb_camera.h"
#include <assert.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "usb_stream.h"

static const char *TAG = "uvc_demo";

#define BIT0_FRAME_START     (0x01 << 0)
#define BIT1_NEW_FRAME_START (0x01 << 1)
#define BIT2_NEW_FRAME_END   (0x01 << 2)

#define DEMO_UVC_FRAME_WIDTH        480
#define DEMO_UVC_FRAME_HEIGHT       320

#ifdef CONFIG_IDF_TARGET_ESP32S2
#define DEMO_UVC_XFER_BUFFER_SIZE (45 * 1024)
#else
#define DEMO_UVC_XFER_BUFFER_SIZE (55 * 1024)
#endif

#define BIT0_FRAME_START     (0x01 << 0)

static void stream_state_changed_cb(usb_stream_state_t event, void *arg) {
    switch (event) {
        case STREAM_CONNECTED: {
            size_t frame_size = 0;
            size_t frame_index = 0;
            uvc_frame_size_list_get(NULL, &frame_size, &frame_index);
            if (frame_size) {
                ESP_LOGI(TAG, "UVC: get frame list size = %u, current = %u", frame_size, frame_index);
                uvc_frame_size_t *uvc_frame_list = (uvc_frame_size_t *) malloc(frame_size * sizeof(uvc_frame_size_t));
                uvc_frame_size_list_get(uvc_frame_list, NULL, NULL);
                for (size_t i = 0; i < frame_size; i++) {
                    ESP_LOGI(TAG, "\tframe[%u] = %ux%u", i, uvc_frame_list[i].width, uvc_frame_list[i].height);
                }
                free(uvc_frame_list);
            } else {
                ESP_LOGW(TAG, "UVC: get frame list size = %u", frame_size);
            }
            ESP_LOGI(TAG, "Device connected");
            break;
        }
        case STREAM_DISCONNECTED:
            ESP_LOGI(TAG, "Device disconnected");
            break;
        default:
            ESP_LOGE(TAG, "Unknown event");
            break;
    }
}

void start() {
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("httpd_txrx", ESP_LOG_INFO);
    esp_err_t ret = ESP_FAIL;
    /* malloc double buffer for usb payload, xfer_buffer_size >= frame_buffer_size*/
    uint8_t *xfer_buffer_a = (uint8_t *) heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE,
                                                          MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    assert(xfer_buffer_a != NULL);
    uint8_t *xfer_buffer_b = (uint8_t *) heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE,
                                                          MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    assert(xfer_buffer_b != NULL);

    /* malloc frame buffer for a jpeg frame*/
    uint8_t *frame_buffer = (uint8_t *) heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE,
                                                         MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    assert(frame_buffer != NULL);

    uvc_config_t uvc_config = {
            /* match the any resolution of current camera (first frame size as default) */
            .frame_width = DEMO_UVC_FRAME_WIDTH,
            .frame_height = DEMO_UVC_FRAME_HEIGHT,
            .frame_interval = FRAME_INTERVAL_FPS_15,
            .xfer_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
            .xfer_buffer_a = xfer_buffer_a,
            .xfer_buffer_b = xfer_buffer_b,
            .frame_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
            .frame_buffer = frame_buffer,
            .frame_cb = &camera_frame_cb,
            .frame_cb_arg = NULL,
    };
    /* config to enable uvc function */
    ret = uvc_streaming_config(&uvc_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uvc streaming config failed");
    }
    /* register the state callback to get connect/disconnect event
    * in the callback, we can get the frame list of current device
    */
    ESP_ERROR_CHECK(usb_streaming_state_register(&stream_state_changed_cb, NULL));
    /* start usb streaming, UVC and UAC MIC will start streaming because SUSPEND_AFTER_START flags not set */
    ESP_ERROR_CHECK(usb_streaming_start());
    ESP_ERROR_CHECK(usb_streaming_connect_wait(portMAX_DELAY));
}
