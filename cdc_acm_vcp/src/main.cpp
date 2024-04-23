#include <Arduino.h>
/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <map>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "usb/cdc_acm_host.h"
#include "usb/vcp_ch34x.hpp"
#include "usb/vcp_cp210x.hpp"
#include "usb/vcp_ftdi.hpp"
#include "usb/vcp.hpp"
#include "usb/usb_host.h"

// Change these values to match your needs
#define EXAMPLE_BAUDRATE     (115200)
#define EXAMPLE_STOP_BITS    (0)      // 0: 1 stopbit, 1: 1.5 stopbits, 2: 2 stopbits
#define EXAMPLE_PARITY       (0)      // 0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
#define EXAMPLE_DATA_BITS    (8)
using namespace esp_usb;
namespace {
    static const char *TAG = "VCP example";
    static SemaphoreHandle_t device_disconnected_sem;

/**
 * @brief Data received callback
 *
 * Just pass received data to stdout
 *
 * @param[in] data     Pointer to received data
 * @param[in] data_len Length of received data in bytes
 * @param[in] arg      Argument we passed to the device open function
 * @return
 *   true:  We have processed the received data
 *   false: We expect more data
 */
    static bool handle_rx(const uint8_t *data, size_t data_len, void *arg) {
        printf("%.*s", data_len, data);
        return true;
    }

/**
 * @brief Device event callback
 *
 * Apart from handling device disconnection it doesn't do anything useful
 *
 * @param[in] event    Device event type and data
 * @param[in] user_ctx Argument we passed to the device open function
 */
    static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx) {
        switch (event->type) {
            case CDC_ACM_HOST_ERROR:
                ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %d", event->data.error);
                break;
            case CDC_ACM_HOST_DEVICE_DISCONNECTED:
                ESP_LOGI(TAG, "Device suddenly disconnected");
                xSemaphoreGive(device_disconnected_sem);
                break;
            case CDC_ACM_HOST_SERIAL_STATE:
                ESP_LOGI(TAG, "Serial state notif 0x%04X", event->data.serial_state.val);
                break;
            case CDC_ACM_HOST_NETWORK_CONNECTION:
            default:
                break;
        }
    }

/**
 * @brief USB Host library handling task
 *
 * @param arg Unused
 */
    static void usb_lib_task(void *arg) {
        while (1) {
            // Start handling system events
            uint32_t event_flags;
            usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
                ESP_ERROR_CHECK(usb_host_device_free_all());
            }
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
                ESP_LOGI(TAG, "USB: All devices freed");
                // Continue handling USB events to allow device reconnection
            }
        }
    }
}

/**
 * @brief Main application
 *
 * This function shows how you can use Virtual COM Port drivers
 */
void app_main() {
    device_disconnected_sem = xSemaphoreCreateBinary();
    assert(device_disconnected_sem);

    // Install USB Host driver. Should only be called once in entire application
    ESP_LOGI(TAG, "Installing USB Host");
    const usb_host_config_t host_config = {
            .skip_phy_setup = false,
            .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    // Create a task that will handle USB library events
    BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, 10, NULL);
    assert(task_created == pdTRUE);

    ESP_LOGI(TAG, "Installing CDC-ACM driver");
    ESP_ERROR_CHECK(cdc_acm_host_install(NULL));

    // Register VCP drivers to VCP service
    VCP::register_driver([](uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx) {
        return static_cast<CdcAcmDevice *>(new FT23x(pid, dev_config, interface_idx));
    }, FT23x::pids(), FT23x::vid);
    VCP::register_driver([](uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx) {
        return static_cast<CdcAcmDevice *>(new CP210x(pid, dev_config, interface_idx));
    }, CP210x::pids(), CP210x::vid);
    VCP::register_driver([](uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx) {
        return static_cast<CdcAcmDevice *>(new CH34x(pid, dev_config, interface_idx));
    }, CH34x::pids(), CH34x::vid);
    std::map<uint16_t, std::map<uint16_t, String>> vendorProductMap{
            {
                    FTDI_VID,                   {{FT232_PID,  "FT232"},           {FT231_PID,   "FT231"}}
            },
            {
                    SILICON_LABS_VID,           {{CP210X_PID, "CP2101 - CP2104"}, {CP2108_PID,  "CP2108"}}
            },
            {
                    NANJING_QINHENG_MICROE_VID, {{CH340_PID,  "CH340"},           {CH340_PID_1, "CH340_1"}, {CH341_PID, "CH341"}}
            }
    };

    // Do everything else in a loop, so we can demonstrate USB device reconnections
    while (true) {
        const cdc_acm_host_device_config_t dev_config = {
                .connection_timeout_ms = 5000, // 5 seconds, enough time to plug the device in or experiment with timeout
                .out_buffer_size = 512,
                .in_buffer_size = 512,
                .event_cb = handle_event,
                .data_cb = handle_rx,
                .user_arg = NULL,
        };

        // You don't need to know the device's VID and PID. Just plug in any device and the VCP service will load correct (already registered) driver for the device
        ESP_LOGI(TAG, "Opening any VCP device...");
        auto vcp = std::unique_ptr<CdcAcmDevice>(VCP::open(&dev_config));
        if (vcp == nullptr) {
            ESP_LOGI(TAG, "Failed to open VCP device");
            continue;
        }
        const usb_device_desc_t *desc = vcp->getDevInfo();
        ESP_LOGI(TAG, "bcdUSB %d.%d0", ((desc->bcdUSB >> 8) & 0xF), ((desc->bcdUSB >> 4) & 0xF));
        ESP_LOGI(TAG, "idVendor 0x%x", desc->idVendor);
        ESP_LOGI(TAG, "idProduct 0x%x", desc->idProduct);
        ESP_LOGI(TAG, "Product is %s", vendorProductMap[desc->idVendor][desc->idProduct].c_str());
        vTaskDelay(10);

        ESP_LOGI(TAG, "Setting up line coding");
        cdc_acm_line_coding_t line_coding = {
                .dwDTERate = EXAMPLE_BAUDRATE,
                .bCharFormat = EXAMPLE_STOP_BITS,
                .bParityType = EXAMPLE_PARITY,
                .bDataBits = EXAMPLE_DATA_BITS,
        };
        ESP_ERROR_CHECK(vcp->line_coding_set(&line_coding));

        /*
        Now the USB-to-UART converter is configured and receiving data.
        You can use standard CDC-ACM API to interact with it. E.g.

        ESP_ERROR_CHECK(vcp->set_control_line_state(false, true));
        ESP_ERROR_CHECK(vcp->tx_blocking((uint8_t *)"Test string", 12));
        */

        // Send some dummy data
        ESP_LOGI(TAG, "Sending data through CdcAcmDevice");
        uint8_t data[] = "test_string";
        ESP_ERROR_CHECK(vcp->tx_blocking(data, sizeof(data)));
        ESP_ERROR_CHECK(vcp->set_control_line_state(true, true));

        // We are done. Wait for device disconnection and start over
        ESP_LOGI(TAG, "Done. You can reconnect the VCP device to run again.");
        xSemaphoreTake(device_disconnected_sem, portMAX_DELAY);
    }
}

void setup() {
    app_main();
}

void loop() {
    delay(10 * 10000);
}