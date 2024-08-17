//
// Created by immor on 2024/8/17.
//

#include "Servo.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void initializeServo() {
    // 导出PWM通道
    ofstream export_file("/sys/class/pwm/pwmchip0/export");
    export_file << 1;
    export_file.close();

    // 设置PWM周期为20,000,000ns (50Hz)
    ofstream period_file("/sys/class/pwm/pwmchip0/pwm1/period");
    period_file << 20000000;
    period_file.close();
}

int calculateDutyCycle(int angle) {
    // 根据角度计算占空比
    const int min_duty = 500000; // 0.5ms
    const int max_duty = 2500000; // 2.5ms
    const int range = max_duty - min_duty;
    return min_duty + (angle * range / 180);
}

void rotateServoToAngle(int angle) {
    // 根据角度计算占空比
    int duty_cycle = calculateDutyCycle(angle);

    // 设置占空比
    ofstream duty_cycle_file("/sys/class/pwm/pwmchip0/pwm1/duty_cycle");
    duty_cycle_file << duty_cycle;
    duty_cycle_file.close();

    // 启用PWM输出
    ofstream enable_file("/sys/class/pwm/pwmchip0/pwm1/enable");
    enable_file << 1;
    enable_file.close();
}
