cmake_minimum_required(VERSION 3.20)

project(VehicleCanMonitoringSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(vehicle_can_monitoring
    src/main.cpp
    src/rpm_message.cpp
    src/engine_node.cpp
    src/dashboard_node.cpp
    src/can_bus.cpp
)

target_include_directories(vehicle_can_monitoring PRIVATE include)
