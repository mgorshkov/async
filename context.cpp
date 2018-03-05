#include <iostream>

#include "context.h"

Context::Context(std::shared_ptr<CommandProcessor> commandProcessor)
    : mThread(ThreadProc, this, commandProcessor)
{
}

void Context::ProcessData(const char* data, std::size_t size)
{
    {
        std::lock_guard<std::mutex> lk(mStreamMutex);
        mStream << std::string(data, size);
    }
    mCondition.notify_one();
}

void Context::Stop()
{
    mDone = true;
    mCondition.notify_one();
    mThread.join();
}

void Context::ProcessStream(std::unique_lock<std::mutex>& lk, std::shared_ptr<CommandProcessor> aCommandProcessor)
{
    std::string line;
    while (std::getline(mStream, line))
        aCommandProcessor->ProcessLine(line);
    lk.unlock();
}

void Context::ThreadProc(Context* aContext, std::shared_ptr<CommandProcessor> aCommandProcessor)
{
    try
    {
        while (!aContext->mDone)
        {
            std::unique_lock<std::mutex> lk(aContext->mStreamMutex);
            aContext->mCondition.wait(lk,
                [&]() ->bool
                {
                    return aContext->mDone;
                });
            aContext->ProcessStream(lk, aCommandProcessor);
        }
        {
            std::unique_lock<std::mutex> lk(aContext->mStreamMutex);
            aContext->ProcessStream(lk, aCommandProcessor);
        }
        aCommandProcessor->Stop();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
