#pragma once
#include "Task.hpp"
#include "DebugLeds.hpp"
#include "mavlink/custom/mavlink.h"
#include "mavlink/custom/mavlink_msg_mission_telemetry.h"
#include "Watchdog/Watchdog.hpp"
#include <cstdio>
#include <array>
#include <span>

template<typename Transport, typename DebugTransport>
class MavlinkRxTask : public Task<512 * sizeof(uint32_t)> {
    private:
        using Base = Task<512 * sizeof(uint32_t)>;
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
            : Base("MavlinkRxTask", TaskPriority::AboveNormal), transport_(transport), debugTransport_(debugTransport) {

        }

        void operator()() override {

            while (true)
            {
                Watchdog::KickTask(Watchdog::TaskId::MavlinkRx);
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
                            debugTransport_.send(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(telemetryMsg_.data()), len));
                        }
                        else
                        {
                            auto len = snprintf(unknownMsg_.data(), unknownMsg_.size(),
                                "[msg %lu] unknown msgid=%u (len=%u)\r\n",
                                (unsigned long)rxCounter_, msg_.msgid, msg_.len);
                            debugTransport_.send(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(unknownMsg_.data()), len));
                        }
                    }
                }

                ActiveOs::Delay(10);
            }
        };

};
