#include <iostream>

#include "commandprocessor.h"

CommandProcessor::CommandProcessor(const std::string& aName, const CommandProcessors& dependentCommandProcessors)
    : mName(aName)
    , mDependentCommandProcessors(dependentCommandProcessors)
{
}

void CommandProcessor::StartBlock()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->StartBlock();

    ++mCounters.mBlockCounter;
}

void CommandProcessor::FinishBlock()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->FinishBlock();
}

void CommandProcessor::ProcessLine(const std::string& line)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->ProcessLine(line);

    ++mCounters.mLineCounter;
}

void CommandProcessor::ProcessCommand(const Command& command)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->ProcessCommand(command);

    ++mCounters.mCommandCounter;
}

void CommandProcessor::ProcessBatch(const CommandBatch& commandBatch)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
    {
        dependentCommandProcessor->ProcessBatch(commandBatch);
    }

    ++mCounters.mBlockCounter;
    mCounters.mCommandCounter += commandBatch.Size();
}

void CommandProcessor::Stop()
{
#ifdef DEBUG_PRINT
    std::cout << "CommandProcessor::Stop1()" << this << std::endl;
#endif
    DumpCounters();
#ifdef DEBUG_PRINT
    std::cout << "CommandProcessor::Stop2()" << this << std::endl;
#endif
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->Stop();
#ifdef DEBUG_PRINT
    std::cout << "CommandProcessor::Stop3()" << this << std::endl;
#endif
}

void CommandProcessor::DumpCounters() const
{
    std::cout << "Thread: " << mName << ", blocks: " << mCounters.mBlockCounter <<
        ", commands: " << mCounters.mCommandCounter;

    if (mCounters.mLineCounter != 0)
        std::cout << ", lines: " << mCounters.mLineCounter;

    std::cout << std::endl;
}
