
#ifndef FACE_RECOGNIZE
#define FACE_RECOGNIZE

#include "esp_err.h"
#include "esp_camera.h"
#include "TFT_eSPI.h"

extern "C" {

typedef enum {
    UNAVAILABLE = -1,
    TARGET_OK = 0,
    TARGET_MIS = 1,
    TARGET_FIND = 2,
    NORMAL = 3,
} face_recognize_status_t;


};
typedef std::function<void(face_recognize_status_t status)> OnRecognizeResult_t;


esp_err_t loop(TFT_eSPI &tft, camera_fb_t *fb);

void initFaceRecognize(OnRecognizeResult_t onRecognizeResult);

/**
 * 读取串口的事件，使用HardwareSerial类中的独立线程
 * */
void serialReceiveTask();

#endif //FACE_RECOGNIZE
