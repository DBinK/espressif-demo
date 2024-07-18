//
// Created by immor on 2024/7/18.
//

#ifndef USB_CAMERA_USB_CAMERA_MIC_SPK_H
#define USB_CAMERA_USB_CAMERA_MIC_SPK_H

#include <Arduino.h>
#include "libuvc_def.h"

#ifdef __cplusplus
extern "C" {
#endif
void start();
void camera_frame_cb(uvc_frame_t *frame, void *ptr);
#ifdef __cplusplus
}
#endif
#endif //USB_CAMERA_USB_CAMERA_MIC_SPK_H
