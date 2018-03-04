#include "threadedcommandprocessor.h"

static std::string Join(const std::vector<Command>& v)
{
    std::stringstream ss;
    for(size_t i = 0; i < v.size(); ++i)
    {
        if(i != 0)
            ss << ", ";
        ss << v[i].mText;
    }
    return ss.str();
}

static Command MakeCommandFromBatch(const CommandBatch& aCommandBatch)
{
    std::string output = "bulk: " + Join(aCommandBatch.mCommands);
    Command command{output, aCommandBatch.mTimestamp};
    return command;
}

/// Starts one thread for each dependent CommandProcessor
template <typename DependentProcessor>
ThreadedCommandProcessor<DependentProcessor>::ThreadedCommandProcessor(const std::string& aName, int aThreads = 1)
    : CommandProcessor("main")
{
    for (int i = 0; i < aThreads; ++i)
    {
        std::stringstream name;
        name << aName << i;
        mThreads.emplace_back(std::thread(ThreadProc, this, name.str()));
    }
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ProcessBatch(const CommandBatch& commandBatch) override
{
    if (mDone)
        return;
    {
        std::lock_guard<std::mutex> lk(mQueueMutex);
        mQueue.push(commandBatch);
    }
    mCondition.notify_all();
    CommandProcessor::ProcessBatch(commandBatch);
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::Stop() override
{
    mDone = true;
    mCondition.notify_all();
    for (auto& thread : mThreads)
        thread.join();
    CommandProcessor::Stop();
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::DumpCounters() const override {}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ThreadProc(ThreadedCommandProcessor* aProcessor, const std::string& aName)
{
    try
    {
        DependentProcessor dependentProcessor(aName);
        while (!aProcessor->mDone)
        {
            std::unique_lock<std::mutex> lk(aProcessor->mQueueMutex);
            aProcessor->mCondition.wait(lk,
                [&]()
                {
                    return !aProcessor->mQueue.empty() || aProcessor->mDone;
                });
            aProcessor->ProcessQueue(lk, dependentProcessor);
        }
        {
            std::unique_lock<std::mutex> lk(aProcessor->mQueueMutex);
            aProcessor->ProcessQueue(lk, dependentProcessor);
        }
        dependentProcessor.Stop();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ProcessQueue(std::unique_lock<std::mutex>& lk, CommandProcessor& aDependentProcessor)
{
    std::queue<CommandBatch> queue;
    std::swap(mQueue, queue);
    lk.unlock();
    while (!queue.empty())
    {
        auto commandBatch = queue.front();
        queue.pop();
        aDependentProcessor.ProcessBatch(commandBatch);
    }
}

