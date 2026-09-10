# STM32 MAVLink Telemetry Firmware

## Overview

FreeRTOS-based MAVLink telemetry firmware for STM32F303ZE written in C++20. It sends and receives mission telemetry over DMA-driven UART and supports automated MAVLink C header generation from `custom.xml`.

## Requirements

* CMake (>= 3.22)
* GNU Arm Embedded Toolchain (`arm-none-eabi-gcc` / `g++`)
* Python with `pymavlink` installed

## Build Instructions

This project supports two RTOS backends. Follow the section that matches the firmware variant you are working on.

### FreeRTOS (CMake)

**1. Configure:**

```bash
cmake -B build -S .
```

> Set `-DMAVLINK_PYTHON` to your Python executable with `pymavlink` installed.

**2. Generate MAVLink headers:**

Required before the initial build to generate C headers from `custom.xml`:

```bash
cmake --build build --target mavlink_generate
```

**3. Build:**

```bash
cmake --build build
```

**4. Flash:**

```bash
STM32_Programmer_CLI -c port=SWD -w build\Debug\blink_blink.elf -v -rst
```

### Zephyr (west)

**1. Build:**

```bash
west build -b nucleo_f303ze -j 8 <path/to/zephyr_app> -d build/blink_blink_zephyr
```

**2. Flash:**

```bash
west flash -d build/blink_blink_zephyr
```
## Monitoring Serial Output
1. Connect the board to your PC via USB.
2. Open a serial monitor on the assigned COM/TTY port at **115200 baud (8N1)** to view decoded logs.
