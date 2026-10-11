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
    src/temperature_message.cpp
)

target_include_directories(vehicle_can_monitoring PRIVATE include)
enable_testing()

add_executable(rpm_message_test
    tests/rpm_message_test.cpp
    src/rpm_message.cpp
)

target_include_directories(rpm_message_test PRIVATE include)

add_test(NAME rpm_message_test COMMAND rpm_message_test)
add_executable(temperature_message_test
    tests/temperature_message_test.cpp
    src/temperature_message.cpp
)

target_include_directories(temperature_message_test PRIVATE include)

add_test(NAME temperature_message_test COMMAND temperature_message_test)
add_executable(can_bus_test
    tests/can_bus_test.cpp
    src/can_bus.cpp
    src/rpm_message.cpp
)

target_include_directories(can_bus_test PRIVATE include)

add_test(NAME can_bus_test COMMAND can_bus_test)
add_executable(dashboard_node_test
    tests/dashboard_node_test.cpp
    src/dashboard_node.cpp
    src/rpm_message.cpp
    src/temperature_message.cpp
)

target_include_directories(dashboard_node_test PRIVATE include)

add_test(NAME dashboard_node_test COMMAND dashboard_node_test)
