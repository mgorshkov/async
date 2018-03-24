#include <assert.h>

#include "inputprocessor.h"

InputProcessor::InputProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors)
    : CommandProcessor(aName, aDependentCommandProcessors)
{
    assert(aDependentCommandProcessors.size() != 0);
}

void InputProcessor::ProcessLine(const std::string& aLine)
{
    if (aLine == "{")
    {
        if (mBlockDepth++ == 0)
        {
            CommandProcessor::StartBlock();
        }
    }
    else if (aLine == "}")
    {
        if (--mBlockDepth == 0)
        {
            CommandProcessor::FinishBlock();
        }
    }
    else
    {
        Command command{aLine, std::chrono::system_clock::now()};
        CommandProcessor::ProcessCommand(command);
    }
    CommandProcessor::ProcessLine(aLine);
}

void InputProcessor::DumpCounters() const
{
}
