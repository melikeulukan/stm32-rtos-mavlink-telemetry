cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME blink_blink)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    App/Src/HardwareInit.cpp
    App/Src/Peripherals/freertos/UartPeripheral.cpp
    App/Src/Peripherals/freertos/PeripheralHandles.cpp

)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    App/Inc
    App/Inc/Peripherals/freertos
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    RTOS_BACKEND_FREERTOS
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

set(MAVLINK_PYTHON "" CACHE FILEPATH "Path to python.exe with pymavlink installed (optional)")

if(MAVLINK_PYTHON)
    get_filename_component(MAVLINK_ENV_DIR ${MAVLINK_PYTHON} DIRECTORY)
    set(MAVGEN_SCRIPT ${MAVLINK_ENV_DIR}/mavgen.py)
    if(EXISTS ${MAVGEN_SCRIPT})
        add_custom_target(mavlink_generate
            COMMAND ${MAVLINK_PYTHON} ${MAVGEN_SCRIPT} --lang=C --wire-protocol=2.0
                    -o ${CMAKE_SOURCE_DIR}/Core/Inc/mavlink
                    ${CMAKE_SOURCE_DIR}/mavlink/dialects/custom.xml
            COMMENT "Regenerating MAVLink headers from custom.xml"
        )
    endif()
endif()