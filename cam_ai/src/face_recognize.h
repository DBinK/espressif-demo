
#ifndef FACE_RECOGNIZE
#define FACE_RECOGNIZE
#include "esp_err.h"
#include "esp_camera.h"
#include "TFT_eSPI.h"

esp_err_t loop(TFT_eSPI& tft, camera_fb_t *fb);
void initFaceRecognize();
void serialReceiveTask();
#endif //FACE_RECOGNIZE
