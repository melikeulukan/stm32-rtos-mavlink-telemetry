#pragma once
#include "Task.hpp"
#include "Watchdog/Watchdog.hpp"

class WatchdogTask : public Task<128 * sizeof(uint32_t)> {
    private:
        using Base = Task<128 * sizeof(uint32_t)>;

    public:
        explicit WatchdogTask()
            : Base("WatchdogTask", TaskPriority::Low) {

        }

        void operator()() override {

            while (true)
            {
                Watchdog::Poll();
                ActiveOs::Delay(1000);
            }
        };

};
