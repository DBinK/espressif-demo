cmake_minimum_required(VERSION 3.22.1)
project(esp_now_linux)

set(CMAKE_CXX_STANDARD 17)

add_executable(esp_now_linux main.cpp ESPNOW_manager.cpp ESPNOW_types.cpp Servo.cpp)
include_directories(Includes)