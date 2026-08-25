#pragma once
#include "cmsis_os2.h"
#include <cstdint>

class Task{
    public:
        explicit Task(const char* name, osPriority_t priority);
        virtual ~Task() = default;

        Task(const Task&) = delete; //copy constructor disabled
        Task& operator=(const Task&) = delete; //copy assignment operator disabled
        virtual void operator()()=0; //Pure Virtual Function

    protected:
        void startDynamic(std::size_t stackBytes);
        void startWithStack(void * stackMem, std::size_t stackBytes);
        
    private:
        const char* name_;
        osPriority_t priority_;
        osThreadId_t handle_{nullptr};

        static void trampoline(void* argument);
};