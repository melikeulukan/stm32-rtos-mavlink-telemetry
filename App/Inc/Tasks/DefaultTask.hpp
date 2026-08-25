#pragma once
#include "Task.hpp"
#include "main.h"

class DefaultTask : public Task{
    public:
        DefaultTask() : Task("defaultTask", 128, osPriorityLow) {

        }

        void operator()() override {

            while(true)
            {
                HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
                osDelay(500);
            } 
        };
};