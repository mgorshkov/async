#include <iostream>

#include "context.h"

Context::Context(std::shared_ptr<CommandProcessor> commandProcessor)
    : mThread(ThreadProc, this, commandProcessor)
{
#ifdef DEBUG_PRINT
    std::cout << "Context::Context, this==" << this << std::endl;
#endif
}

Context::~Context()
{
#ifdef DEBUG_PRINT
    std::cout << "Context::~Context, this==" << this << std::endl;
#endif
    Stop();
}

void Context::ProcessData(const char* data, std::size_t size)
{
    if (mDone.load())
        return;
    {
        std::lock_guard<std::mutex> lk(mStreamMutex);
        mStream << std::string(data, size);
    }
    mNotified = true;
    mCondition.notify_one();
}

void Context::Stop()
{
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop1, this==" << this << std::endl;
#endif
    mDone = true;
    mNotified = true;
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop2, this==" << this << std::endl;
#endif
    mCondition.notify_one();
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop3, this==" << this << std::endl;
#endif
    if (mThread.joinable())
        mThread.join();
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop4, this==" << this << std::endl;
#endif
}

bool Context::GetNextLine(std::string& line)
{
    std::lock_guard<std::mutex> lk(mStreamMutex);
    return !!std::getline(mStream, line);
}

void Context::ProcessStream(std::shared_ptr<CommandProcessor> aCommandProcessor)
{
    std::string line;
    while (true)
    {
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStream 1" << std::endl;
#endif
        std::string linePart;
        bool res = GetNextLine(linePart);
        if (linePart.empty())
            return;
        if (res)
        {
            line += linePart;
            aCommandProcessor->ProcessLine(line);
#ifdef DEBUG_PRINT
            std::cout << "Context::ProcessStream 2, line=" << line << std::endl;
#endif
            return;
        }
        line += linePart;
    }
}

void Context::ProcessStreamFinal(std::shared_ptr<CommandProcessor> aCommandProcessor)
{
    std::string line;
    while (GetNextLine(line))
    {
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStreamFinal" << line << std::endl;
#endif
        aCommandProcessor->ProcessLine(line);
    }
}

void Context::ThreadProc(Context* aContext, std::shared_ptr<CommandProcessor> aCommandProcessor)
{
#ifdef DEBUG_PRINT
    std::cout << "Context::ThreadProc start, this==" << aContext << std::endl;
#endif
    try
    {
        while (!aContext->mDone.load())
        {
            std::unique_lock<std::mutex> lk(aContext->mStreamMutex);
            while (!aContext->mNotified.load())
                aContext->mCondition.wait(lk);
            lk.unlock();
            aContext->ProcessStream(aCommandProcessor);
            aContext->mNotified = false;
        }
#ifdef DEBUG_PRINT
        std::cout << "Context::ThreadProc1, this==" << aContext << std::endl;
#endif
        
        aContext->ProcessStreamFinal(aCommandProcessor);

#ifdef DEBUG_PRINT
        std::cout << "Context::ThreadProc2, this==" << aContext << std::endl;
#endif
        aCommandProcessor->Stop();
#ifdef DEBUG_PRINT
        std::cout << "Context::ThreadProc3, this==" << aContext << std::endl;
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
#ifdef DEBUG_PRINT
    std::cout << "Context::ThreadProc end, this==" << aContext << std::endl;
#endif
}
