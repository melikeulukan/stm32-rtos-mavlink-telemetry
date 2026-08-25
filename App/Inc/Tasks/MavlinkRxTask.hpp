#pragma once
#include "Task.hpp"
#include "DebugLeds.hpp"
#include "mavlink/custom/mavlink.h"
#include "mavlink/custom/mavlink_msg_mission_telemetry.h"
#include <cstdio>
#include <array>

#if defined(RTOS_BACKEND_ZEPHYR)
#include <zephyr/kernel.h>
K_THREAD_STACK_DEFINE(mavlink_rx_stack, 512 * sizeof(uint32_t));
#endif

template<typename Transport, typename DebugTransport>
class MavlinkRxTask : public Task {
    private:
        Transport& transport_;
        DebugTransport& debugTransport_;
        uint32_t rxCounter_{0};

        mavlink_message_t msg_{};
        mavlink_status_t status_{};
        mavlink_mission_telemetry_t telemetry_{};
        std::array<char, 320> telemetryMsg_{};
        std::array<char, 64> unknownMsg_{};

    public:
        explicit MavlinkRxTask(Transport& transport, DebugTransport& debugTransport)
            : Task("MavlinkRxTask", osPriorityAboveNormal), transport_(transport), debugTransport_(debugTransport) {

        }

#if defined(RTOS_BACKEND_ZEPHYR)
        void start() { startWithStack(mavlink_rx_stack, K_THREAD_STACK_SIZEOF(mavlink_rx_stack)); }
#elif defined(RTOS_BACKEND_FREERTOS)
        void start() { startDynamic(512 * sizeof(uint32_t)); }
#endif

        void operator()() override {

            while (true)
            {
                uint16_t receivedLen;
                const uint8_t* packet = transport_.receive(receivedLen);

                for (uint16_t i = 0; i < receivedLen; ++i)
                {
                    if (mavlink_parse_char(MAVLINK_COMM_0, packet[i], &msg_, &status_) == 1)
                    {
                        rxCounter_++;

                        if (msg_.msgid == MAVLINK_MSG_ID_MISSION_TELEMETRY)
                        {
                            mavlink_msg_mission_telemetry_decode(&msg_, &telemetry_);

                            IndicateRxActivity();
                            
                            auto len = snprintf(telemetryMsg_.data(), telemetryMsg_.size(),
                                "[msg %lu] MISSION_TELEMETRY t=%lu seq=%u state=%u alt=%.1f temp=%.1f press=%.1f "
                                "gyro=(%.2f,%.2f,%.2f) accel=(%.2f,%.2f,%.2f) gps=(%ld,%ld) sats=%u\r\n",
                                (unsigned long)rxCounter_,
                                (unsigned long)telemetry_.mission_time,
                                telemetry_.packet_count,
                                telemetry_.state,
                                telemetry_.altitude,
                                telemetry_.temperature,
                                telemetry_.pressure,
                                telemetry_.gyro_r,
                                telemetry_.gyro_p,
                                telemetry_.gyro_y,
                                telemetry_.accel_r,
                                telemetry_.accel_p,
                                telemetry_.accel_y,
                                (long)telemetry_.gps_latitude,
                                (long)telemetry_.gps_longitude,
                                telemetry_.gps_sats);
                            debugTransport_.send(reinterpret_cast<const uint8_t*>(telemetryMsg_.data()), len);
                        }
                        else
                        {
                            auto len = snprintf(unknownMsg_.data(), unknownMsg_.size(),
                                "[msg %lu] unknown msgid=%u (len=%u)\r\n",
                                (unsigned long)rxCounter_, msg_.msgid, msg_.len);
                            debugTransport_.send(reinterpret_cast<const uint8_t*>(unknownMsg_.data()), len);
                        }
                    }
                }

                osDelay(10);
            }
        };

};
