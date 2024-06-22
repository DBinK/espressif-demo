#include <Arduino.h>

#include "driver/i2c.h"
#include "mpu6050.h"
#include "esp_system.h"
#include "esp_log.h"

#define I2C_MASTER_NUM I2C_NUM_0  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */

static const char *TAG = "mpu6050 test";
static mpu6050_handle_t mpu6050 = nullptr;

/**
 * @brief i2c master initialization
 */
static void i2c_bus_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t) MPU_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t) MPU_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

}

/**
 * @brief i2c master initialization
 */
static void i2c_sensor_mpu6050_init() {
    i2c_bus_init();
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    if (!mpu6050) {
        ESP_LOGE(TAG, "MPU6050 create returned NULL");
        abort();
    }

    ESP_ERROR_CHECK(mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS));

    ESP_ERROR_CHECK(mpu6050_wake_up(mpu6050));
}

void setup() {
    uint8_t mpu6050_deviceid;
    i2c_sensor_mpu6050_init();
    ESP_ERROR_CHECK(mpu6050_get_deviceid(mpu6050, &mpu6050_deviceid));
    if (MPU6050_WHO_AM_I_VAL != mpu6050_deviceid) {
        ESP_LOGE(TAG, "Who Am I register does not contain expected data");
        abort();
    }
}

void deleteMpu6050() {
    mpu6050_delete(mpu6050);
    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
}

void loop() {
    esp_err_t ret;
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;
    mpu6050_temp_value_t temp;
    ret = mpu6050_get_acce(mpu6050, &acce);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "mpu6050_get_acce failed: %d", ret);
        return;
    }
    ESP_LOGI(TAG, "acce_x:%.2f, acce_y:%.2f, acce_z:%.2f\n", acce.acce_x, acce.acce_y, acce.acce_z);

    ret = mpu6050_get_gyro(mpu6050, &gyro);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "mpu6050_get_gyro failed: %d", ret);
        return;
    }

    ESP_LOGI(TAG, "gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f\n", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
    ret = mpu6050_get_temp(mpu6050, &temp);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "mpu6050_get_temp failed: %d", ret);
        return;
    }
    ESP_LOGI(TAG, "t:%.2f \n", temp.temp);
    delay(50);
}