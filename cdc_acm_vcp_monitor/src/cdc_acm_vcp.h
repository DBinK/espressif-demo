//
// Created by immor on 2024/5/3.
//

#ifndef CDC_ACM_VCP_MONITOR_CDC_ACM_VCP_H
#define CDC_ACM_VCP_MONITOR_CDC_ACM_VCP_H
/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
// 打日志需要Arduino.h
#include <Arduino.h>
#include <cstdio>
#include <cstring>

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
#ifndef VCP_BAUDRATE
#define VCP_BAUDRATE     (115200)
#endif
#ifndef VCP_STOP_BITS
#define VCP_STOP_BITS    (0)      // 0: 1 stopbit, 1: 1.5 stopbits, 2: 2 stopbits
#endif
#ifndef VCP_PARITY
#define VCP_PARITY       (0)      // 0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
#endif
#ifndef VCP_DATA_BITS
#define VCP_DATA_BITS    (8)
#endif

class CdcAcmVcp {
public:
   void start(cdc_acm_line_coding_t acmLineCoding, cdc_acm_data_callback_t dataCallback);
private:

};


#endif //CDC_ACM_VCP_MONITOR_CDC_ACM_VCP_H
