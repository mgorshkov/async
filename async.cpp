#include <map>
#include <sstream>

#include "threadedcommandprocessor.h"
#include "consoleoutput.h"
#include "inputprocessor.h"
#include "batchcommandprocessor.h"
#include "reportwriter.h"

#define THREADS 2

#include "async.h"

class Context
{
public:
    Context(std::shared_ptr<CommandProcessor> commandProcessor)
        : mThread(ThreadProc, this, commandProcessor)
    {
    }

    void ProcessData(const char* data, std::size_t size)
    {
        {
            std::lock_guard<std::mutex> lk(mStreamMutex);
            mStream << std::string(data, size);
        }
        mCondition.notify_one();
    }

    void ProcessStream(std::unique_lock<std::mutex>& lk, CommandProcessor& aCommandProcessor)
    {
        std::string line;
        while (std::getline(mStream, line))
            aCommandProcessor.ProcessLine(line);
        lk.unlock();
    }

    void Stop()
    {
        mDone = true;
        mCondition.notify_one();
        mThread.join();
    }

private:
    static void ThreadProc(Context* aContext, std::shared_ptr<CommandProcessor> aCommandProcessor)
    {
        try
        {
            while (!aContext->mDone)
            {
                std::unique_lock<std::mutex> lk(aContext->mStreamMutex);
                aContext->mCondition.wait(lk,
                    [&]()
                    {
                        return aContext->mDone;
                    });
                aContext->ProcessStream(lk, aCommandProcessor);
            }
            {
                std::unique_lock<std::mutex> lk(aContext->mQueueMutex);
                aContext->ProcessStream(lk, aCommandProcessor);
            }
            aCommandProcessor.Stop();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    std::thread mThread;
    std::stringstream mStream;
    std::mutex mStreamMutex;
    std::condition_variable mCondition;
    std::atomic<bool> mDone{false};
};

using ContextMap = std::map<async::handle_t, Context>;

static ContextMap Contexts;

static std::shared_ptr<CommandProcessor> CreateInputContext(std::size_t bulkSize)
{
    auto reportWritersThreadedProcessor = std::make_shared<ThreadedCommandProcessor<ReportWriter>>("file", THREADS);
    auto consoleOutputThreadedProcessor = std::make_shared<ThreadedCommandProcessor<ConsoleOutput>>("log");

    CommandProcessors processors = {reportWritersThreadedProcessor, consoleOutputThreadedProcessor};
    auto batchCommandProcessor = std::make_shared<BatchCommandProcessor>(bulkSize, processors);
    CommandProcessors batchCommandProcessors = {batchCommandProcessor};
    auto inputCommandProcessor = std::make_shared<InputProcessor>(batchCommandProcessors);
    return inputCommandProcessor;
}

static async::handle_t RegisterContext(std::shared_ptr<CommandProcessor> commandProcessor)
{
    Contexts[commandProcessor.get()] = Context(commandProcessor);
    return commandProcessor.get();
}

static Context* FindContext(async::handle_t context)
{
    auto it = Contexts.find(context);
    if (it != Contexts.end())
        return &*it;

    return nullptr;
}

static void RunBulk(handle_t handle, const char *data, std::size_t size)
{
    auto context = FindContext(handle);
    if (!context)
        return;
    context->ProcessData(data, size);
}

static void StopBulk(handle_t handle)
{
    auto context = FindContext(handle);
    if (!context)
        return;
    context->Stop();
    Contexts.erase(*context);
}

namespace async
{

handle_t connect(std::size_t bulkSize)
{
    try
    {
        auto inputContext = CreateInputContext(bulkSize);
        return RegisterContext(inputContext);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return nullptr;
}

void receive(handle_t handle, const char *data, std::size_t size)
{
    try
    {
        RunBulk(handle, data, size);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void disconnect(handle_t handle)
{
    try
    {
        StopBulk(handle);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

}
