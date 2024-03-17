#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "esp32-hal-log.h"

#include <vector>
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"


#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#include "face_recognition_tool.hpp"
#include "face_recognition_112_v1_s8.hpp"
#include "TFT_eSPI.h"

#pragma GCC diagnostic error "-Wformat"
#pragma GCC diagnostic warning "-Wstrict-aliasing"

#define FACE_ID_SAVE_NUMBER 7

#define FACE_COLOR_WHITE 0x00FFFFFF
#define FACE_COLOR_BLACK 0x00000000
#define FACE_COLOR_RED TFT_RED
#define FACE_COLOR_GREEN TFT_GREEN
#define FACE_COLOR_BLUE TFT_BLUE
#define FACE_COLOR_CYAN TFT_CYAN

static int8_t recognition_enabled = 1;
static int8_t is_enrolling = 0;

FaceRecognition112V1S8 recognizer;

HumanFaceDetectMSR01 s1(0.1F, 0.5F, 10, 0.2F);
HumanFaceDetectMNP01 s2(0.5F, 0.3F, 5);

typedef std::list<dl::detect::result_t> DetectResultList;

extern "C" {
typedef struct {
    int width;
    int height;
    uint8_t *data;
} frame_buffer;
}

static void rgb_print(TFT_eSPI &tft, frame_buffer *fb, uint32_t color, const char *str) {
    tft.setTextColor(color);
    tft.drawString(str, (fb->width - (strlen(str) * 14)) / 2, 10);
}

static int rgb_printf(TFT_eSPI &tft, frame_buffer *fb, uint32_t color, const char *format, ...) {
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf)) {
        temp = (char *) malloc(len + 1);
        if (temp == nullptr) {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    rgb_print(tft, fb, color, temp);
    if (len > 64) {
        free(temp);
    }
    return len;
}

static void
draw_face_boxes(TFT_eSPI &tft, size_t out_width, size_t out_height, DetectResultList *results,
                int face_id) {
    int x, y, w, h;
    uint32_t color = TFT_YELLOW;
    if (face_id < 0) {
        color = FACE_COLOR_RED;
    } else if (face_id > 0) {
        color = FACE_COLOR_GREEN;
    }
    int i = 0;
    for (auto prediction = results->begin(); prediction != results->end();
         prediction++, i++) {
        // rectangle box
        x = (int) prediction->box[0];
        y = (int) prediction->box[1];
        w = (int) prediction->box[2] - x + 1;
        h = (int) prediction->box[3] - y + 1;
        if ((x + w) > out_width) {
            w = out_width - x;
        }
        if ((y + h) > out_height) {
            h = out_height - y;
        }
        tft.drawFastHLine(x, y, w, color);
        tft.drawFastHLine(x, y, w, color);
        tft.drawFastHLine(x, y + h - 1, w, color);
        tft.drawFastVLine(x, y, h, color);
        tft.drawFastVLine(x + w - 1, y, h, color);

        // landmarks (left eye, mouth left, nose, right eye, mouth right)
        int x0, y0, j;
        for (j = 0; j < 10; j += 2) {
            x0 = (int) prediction->keypoint[j];
            y0 = (int) prediction->keypoint[j + 1];
            tft.fillRect(x0, y0, 3, 3, color);
        }
    }
}


static int run_face_recognition(TFT_eSPI &tft, frame_buffer *frameBuffer, DetectResultList *results) {
    std::vector<int> landmarks = results->front().keypoint;
    int id = -1;

    Tensor<uint8_t> tensor;
    tensor.set_element((uint8_t *) frameBuffer->data).set_shape(
            {frameBuffer->height, frameBuffer->width, 3}).set_auto_free(false);

    int enrolled_count = recognizer.get_enrolled_id_num();

    if (enrolled_count < FACE_ID_SAVE_NUMBER && is_enrolling) {
        id = recognizer.enroll_id(tensor, landmarks, "", true);
        log_i("Enrolled ID: %d", id);
        rgb_printf(tft, frameBuffer, FACE_COLOR_CYAN, "ID[%u]", id);
    }

    face_info_t recognize = recognizer.recognize(tensor, landmarks);
    if (recognize.id >= 0) {
        rgb_printf(tft, frameBuffer, FACE_COLOR_GREEN, "ID[%u]: %.2f", recognize.id, recognize.similarity);
    } else {
        rgb_print(tft, frameBuffer, FACE_COLOR_RED, "Intruder Alert!");
    }
    return recognize.id;
}


esp_err_t loop(TFT_eSPI &tft, camera_fb_t *fb) {
    esp_err_t res = ESP_OK;
    int face_id;
    size_t out_len, out_width, out_height;
    uint8_t *out_buf;
    face_id = 0;
    out_len = fb->width * fb->height * 3;
    out_width = fb->width;
    out_height = fb->height;
    out_buf = (uint8_t *) malloc(out_len);
    if (!out_buf) {
        log_e("out_buf malloc failed");
        res = ESP_FAIL;
        return res;
    }
    size_t len = fb->len;
    // 转rgb888之后检测， 如果转rgb565检测需要将infer 第一个参数转为uint16指针
    if (!fmt2rgb888(fb->buf, len, fb->format, out_buf)) {
        free(out_buf);
        log_e("To rgb888 failed");
        res = ESP_FAIL;
        return res;
    }
    DetectResultList &candidates = s1.infer(
            (uint8_t *) out_buf, {(int) out_height, (int) out_width, 3});
    DetectResultList &results = s2.infer(
            (uint8_t *) out_buf, {(int) out_height, (int) out_width, 3}, candidates);

    jpg2rgb565(fb->buf, len, out_buf, JPG_SCALE_NONE);
    esp_camera_fb_return(fb);
    tft.startWrite();
    tft.pushImage(0, 0, TFT_HEIGHT, TFT_WIDTH, (uint16_t *) out_buf);
    // 如果检测到了人脸会识别 并框选人脸
    if (!results.empty()) {
        if (recognition_enabled) {
            frame_buffer frameBuffer = {
                    .width = static_cast<int>(out_width),
                    .height = static_cast<int>(out_height),
                    .data = out_buf
            };
            face_id = run_face_recognition(tft, &frameBuffer, &results);
        }
        draw_face_boxes(tft, out_width, out_height, &results, face_id);
    }
    tft.endWrite();
    free(out_buf);
    return res;
}

void initFaceRecognize() {
    recognizer.set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    // load ids from flash partition
    recognizer.set_ids_from_flash();
}

void serialReceiveTask() {
    uint8_t buffer[2] = {};
    size_t len = Serial.readBytesUntil('\n', buffer, 1);
    if (len > 0) {
        switch (buffer[0]) {
            case 'R':
            case 'r':
                recognition_enabled = 1;
                log_i("%c\nrecognition enabled", buffer[0]);
                break;
            case 'd':
            case 'D':
                recognition_enabled = 0;
                log_i("%c\nrecognition disabled", buffer[0]);
                break;
            case 'e':
            case 'E':
                is_enrolling = 1;
                log_i("%c\nenrolling enabled", buffer[0]);
                break;
            default:
                break;
        }
    }

}