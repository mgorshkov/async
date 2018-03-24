#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>

#include "commandprocessor.h"

/// Starts one thread for each dependent CommandProcessor
template <typename DependentProcessor>
class ThreadedCommandProcessor : public CommandProcessor
{
public:
    ThreadedCommandProcessor(const std::string& aName, int aThreadsCount = 1);
    ~ThreadedCommandProcessor();

    void Start() override;

    void ProcessBatch(const CommandBatch& aCommandBatch) override;

    void Stop() override;

    void DumpCounters() const override;

private:
    static void ThreadProc(ThreadedCommandProcessor* aProcessor, const std::string& aName);

    void ProcessQueue(std::unique_lock<std::mutex>& lk, CommandProcessor& aDependentProcessor);

    int mThreadsCount;
    std::queue<CommandBatch> mQueue;
    std::condition_variable mCondition;
    std::mutex mQueueMutex;
    std::atomic_bool mDone{false};

    std::vector<std::thread> mThreads;
};
