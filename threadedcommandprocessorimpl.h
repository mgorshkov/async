#include "threadedcommandprocessor.h"

/// Starts one thread for each dependent CommandProcessor
template <typename DependentProcessor>
ThreadedCommandProcessor<DependentProcessor>::ThreadedCommandProcessor(const std::string& aName, int aThreads)
    : CommandProcessor("main")
{
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor::ThreadedCommandProcessor, this==" << this << std::endl;
#endif
    for (int i = 0; i < aThreads; ++i)
    {
        std::stringstream name;
        name << aName << i;
        mThreads.emplace_back(std::thread(ThreadProc, this, name.str()));
    }
}

template<typename DependentProcessor>
ThreadedCommandProcessor<DependentProcessor>::~ThreadedCommandProcessor()
{
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor::~ThreadedCommandProcessor, this==" << this << std::endl;
#endif
    Stop();
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ProcessBatch(const CommandBatch& commandBatch)
{
    if (mDone.load())
        return;
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor notified - start, this==" << this << std::endl;
#endif
    {
        std::lock_guard<std::mutex> lk(mQueueMutex);
        mQueue.push(commandBatch);
    }
    mNotified = true;
    mCondition.notify_all();
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor notified, this==" << this << std::endl;
#endif
    CommandProcessor::ProcessBatch(commandBatch);
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::Stop()
{
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop()" << this << std::endl;
#endif
    mDone = true;
    mNotified = true;
    mCondition.notify_all();
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop2()" << this << std::endl;
#endif
    for (auto& thread : mThreads)
        if (thread.joinable())
            thread.join();
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop3()" << this << std::endl;
#endif
    CommandProcessor::Stop();
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop4()" << this << std::endl;
#endif
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::DumpCounters() const {}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ThreadProc(ThreadedCommandProcessor* aProcessor, const std::string& aName)
{
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor::ThreadProc start, this==" << aProcessor << std::endl;
#endif
    try
    {
        DependentProcessor dependentProcessor(aName);
        while (!aProcessor->mDone.load())
        {
            std::unique_lock<std::mutex> lk(aProcessor->mQueueMutex);
            while (aProcessor->mQueue.empty() && !aProcessor->mNotified.load())
                aProcessor->mCondition.wait(lk);
            aProcessor->ProcessQueue(lk, dependentProcessor);
            aProcessor->mNotified = false;
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
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor::ThreadProc end, this==" << aProcessor << std::endl;
#endif
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

