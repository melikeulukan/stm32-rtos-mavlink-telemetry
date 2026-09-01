#pragma once
#include "Os/ActiveOs.hpp"
#include <cstddef>
#include <cstdint>

template<std::size_t StackBytes>

class Task{
    public:
        explicit Task(const char* name, TaskPriority priority)
            : name_(name), priority_(ActiveOs::ToNativePriority(priority)){
        }
        virtual ~Task() = default;

        Task(const Task&) = delete; //copy constructor disabled
        Task& operator=(const Task&) = delete; //copy assignment operator disabled
        virtual void operator()()=0; //Pure Virtual Function

        void start()
        {
            handle_ = ActiveOs::CreateThread<StackBytes>(name_,
                 priority_, &trampoline, this);
        }

    private:
        const char* name_;
        ActiveOs::Priority priority_;
        ActiveOs::ThreadHandle handle_{};

        static void trampoline(void* argument)
        {
            auto* self = static_cast<Task*>(argument);
            if (self != nullptr) { (*self)(); }
            ActiveOs::TerminateCurrentThread();
        }
};