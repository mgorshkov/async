#include <map>
#include <sstream>
#include <iostream>

#include "threadedcommandprocessor.h"
#include "threadedcommandprocessorimpl.h"
#include "consoleoutput.h"
#include "inputprocessor.h"
#include "batchcommandprocessor.h"
#include "reportwriter.h"
#include "context.h"

#define THREADS 2

#include "async.h"

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

using ContextMap = std::map<async::handle_t, std::shared_ptr<Context>>;

static ContextMap Contexts;
static std::mutex ContextMutex;

static async::handle_t RegisterContext(std::shared_ptr<CommandProcessor> commandProcessor)
{
    std::lock_guard<std::mutex> lk(ContextMutex);
    Contexts.emplace(std::make_pair(commandProcessor.get(), std::make_shared<Context>(commandProcessor)));
    return commandProcessor.get();
}

static void RunBulk(async::handle_t handle, const char *data, std::size_t size)
{
    std::shared_ptr<Context> context;
    {
        std::lock_guard<std::mutex> lk(ContextMutex);
        auto it = Contexts.find(handle);
        if (it == Contexts.end())
            return;
        context = it->second;
    }
    context->ProcessData(data, size);
}

static void StopBulk(async::handle_t handle)
{
    std::shared_ptr<Context> context;
    {
        std::lock_guard<std::mutex> lk(ContextMutex);
        auto it = Contexts.find(handle);
        if (it == Contexts.end())
            return;
        context = it->second;
    }
    context->Stop();
    {
        std::lock_guard<std::mutex> lk(ContextMutex);
        auto it = Contexts.find(handle);
        if (it == Contexts.end())
            return;
        Contexts.erase(it);
    }
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
