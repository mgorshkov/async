#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <sstream>
#include <condition_variable>
#include <atomic>

#include "commandprocessor.h"

class Context
{
public:
    Context(std::shared_ptr<CommandProcessor> commandProcessor);

    void ProcessData(const char* data, std::size_t size);

    void Stop();

private:
    void ProcessStream(std::unique_lock<std::mutex>& lk, std::shared_ptr<CommandProcessor> aCommandProcessor);

    static void ThreadProc(Context* aContext, std::shared_ptr<CommandProcessor> aCommandProcessor);

    std::thread mThread;
    std::stringstream mStream;
    std::mutex mStreamMutex;
    std::condition_variable mCondition;
    std::atomic<bool> mDone{false};
};
