#include "threadedcommandprocessor.h"

/// Starts one thread for each dependent CommandProcessor
template <typename DependentProcessor>
ThreadedCommandProcessor<DependentProcessor>::ThreadedCommandProcessor(const std::string& aName, int aThreadsCount)
    : CommandProcessor(aName)
    , mThreadsCount(aThreadsCount)
{
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor::ThreadedCommandProcessor, this==" << this << std::endl;
#endif
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
void ThreadedCommandProcessor<DependentProcessor>::Start()
{
    for (int i = 0; i < mThreadsCount; ++i)
    {
        std::stringstream name;
        name << mName << i;
        mThreads.emplace_back(std::thread(ThreadProc, this, name.str()));
    }
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ProcessBatch(const CommandBatch& aCommandBatch)
{
    if (mDone.load())
        return;
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor notified - start, this==" << this << std::endl;
#endif
    {
        std::lock_guard<std::mutex> lk(mQueueMutex);
        mQueue.push(aCommandBatch);
    }
    mCondition.notify_all();
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor notified, this==" << this << std::endl;
#endif
    CommandProcessor::ProcessBatch(aCommandBatch);
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::Stop()
{
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop()" << this << std::endl;
#endif
    mDone = true;
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
        dependentProcessor.SetContext(aProcessor->mContext);
#ifdef DEBUG_PRINT
        std::cout << "ThreadedCommandProcessor::ThreadProc 1, this==" << aProcessor << " " << aProcessor->mContext << std::endl;
#endif
        while (!aProcessor->mDone.load())
        {
            std::unique_lock<std::mutex> lk(aProcessor->mQueueMutex);
            while (aProcessor->mQueue.empty() && !aProcessor->mDone.load())
                aProcessor->mCondition.wait(lk);
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

