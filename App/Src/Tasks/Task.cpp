#include "Tasks/Task.hpp"
#include <cstdio>

Task::Task(const char* name, osPriority_t priority)
    : name_(name), priority_(priority){
}

void Task::startDynamic(std::size_t stackBytes)
{
    osThreadAttr_t attr = {};
    attr.name = name_;
    attr.priority = priority_;
    attr.stack_size = stackBytes;

    handle_ = osThreadNew(trampoline, this, &attr);
}

void Task::startWithStack(void* stackMem, std::size_t stackBytes)
{
    osThreadAttr_t attr = {};
    attr.name = name_;
    attr.stack_mem = stackMem;
    attr.stack_size = stackBytes;
    attr.priority = priority_;

    handle_ = osThreadNew(trampoline, this, &attr);
}

void Task::trampoline(void* argument)
{
    auto* self = static_cast<Task*>(argument);
    
    if(self != nullptr)
    {
        (*self)();
    }

    osThreadTerminate(osThreadGetId());
}