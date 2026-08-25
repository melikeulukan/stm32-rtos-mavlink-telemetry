#pragma once
#include "Task.hpp"
#include "DebugLeds.hpp"
#include <cstdint>
#include <cstddef>
#include <array>
#include "mavlink/custom/mavlink.h"
#include "mavlink/custom/mavlink_msg_mission_telemetry.h"
#include "mavlink/mavlink_types.h"
#include "MavlinkDummyData.hpp"

#if defined(RTOS_BACKEND_ZEPHYR)
#include <zephyr/kernel.h>
K_THREAD_STACK_DEFINE(mavlink_tx_stack, 256 * sizeof(uint32_t));
#endif

template<typename Transport>

class MavlinkTxTask : public Task {
    private:
        Transport& transport_;
        std::array<uint8_t, MAVLINK_MAX_PACKET_LEN> mavBuf_{};
        mavlink_message_t msg_{};
        uint8_t idx_{0};
        static constexpr uint8_t kSystemId = 1;
        static constexpr uint8_t kComponentId = 1;

    public:
        explicit MavlinkTxTask(Transport& transport)
            : Task("MavlinkTxTask", osPriorityNormal), transport_(transport){

        }

#if defined(RTOS_BACKEND_ZEPHYR)
        void start() { startWithStack(mavlink_tx_stack, K_THREAD_STACK_SIZEOF(mavlink_tx_stack)); }
#elif defined(RTOS_BACKEND_FREERTOS)
        void start() { startDynamic(256 * sizeof(uint32_t)); }
#endif

        void operator()() override {
            constexpr std::size_t kDatasetSize = std::size(dummy_dataset);
            printf("tx task running\r\n");

            while (true) {
                mavlink_msg_mission_telemetry_encode(kSystemId, kComponentId, &msg_, &dummy_dataset[idx_]);
                auto len = mavlink_msg_to_send_buffer(mavBuf_.data(), &msg_);
                transport_.send(mavBuf_.data(), len);

                IndicateTxActivity();
                idx_ = (idx_ + 1) % kDatasetSize;

                osDelay(1000);
            }
        }

};