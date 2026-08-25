cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blink_blink_zephyr)

target_compile_definitions(app PRIVATE RTOS_BACKEND_ZEPHYR)

target_sources(app PRIVATE
    ../App/Src/main.cpp
    ../App/Src/HardwareInit.cpp
    ../App/Src/Peripherals/zephyr/UartPeripheral.cpp
    ../App/Src/Peripherals/zephyr/PeripheralHandles.cpp
)

target_include_directories(app PRIVATE
    ../App/Inc
    ../App/Inc/Peripherals/zephyr
    ../Core/Inc
    ${ZEPHYR_BASE}/include/zephyr/portability
)