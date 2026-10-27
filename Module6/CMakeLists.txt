cmake_minimum_required(VERSION 3.16)
project(DigitalTwinLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

add_executable(DigitalTwinLab
    main.cpp
    mainwindow.h         mainwindow.cpp
    LoginWindow.h        LoginWindow.cpp

    # ── New GUI layer ──────────────────────────────────────────
    UIController.h           UIController.cpp
    DeviceDashboard.h        DeviceDashboard.cpp
    DroneControlPanel.h      DroneControlPanel.cpp
    SimulationControlWindow.h SimulationControlWindow.cpp

    # ── Module 1: Utility classes ──────────────────────────────
    ../Module1/Battery.cpp
    ../Module1/Coordinate.cpp
    ../Module1/TelemetryData.cpp

    # ── Module 2: Device hierarchy ─────────────────────────────
    ../Module2/Device.cpp
    ../Module2/AutonomousDevice.cpp
    ../Module2/Drone.cpp
    ../Module2/SurveillanceDrone.cpp
    ../Module2/Sensor.cpp
    ../Module2/EnvironmentalSensor.cpp
    ../Module2/ClimateSensor.cpp

    # ── Module 3: Simulation engine ────────────────────────────
    ../Module3/SimulationEngine.cpp

    # ── Module 4: Controllers ──────────────────────────────────
    ../Module4/DeviceController.cpp
    ../Module4/SensorController.cpp
    ../Module4/SimulationController.cpp

    # ── Module 7: File Persistence ─────────────────────────────
    ../Module7/DataManager.cpp
)

target_link_libraries(DigitalTwinLab PRIVATE Qt6::Widgets Qt6::Core)

target_include_directories(DigitalTwinLab PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../Module7/Source
)
