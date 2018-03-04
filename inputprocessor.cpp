#include "inputprocessor.h"

InputProcessor::InputProcessor(const CommandProcessors& dependentCommandProcessors)
    : CommandProcessor("main", dependentCommandProcessors)
{
    assert(dependentCommandProcessors.size() != 0);
}

void InputProcessor::ProcessLine(const std::string& line) override
{
    if (line == "{")
    {
        if (mBlockDepth++ == 0)
        {
            CommandProcessor::StartBlock();
        }
    }
    else if (line == "}")
    {
        if (--mBlockDepth == 0)
        {
            CommandProcessor::FinishBlock();
        }
    }
    else
    {
        Command command{line, std::chrono::system_clock::now()};
        CommandProcessor::ProcessCommand(command);
    }
    CommandProcessor::ProcessLine(line);
}

void InputProcessor::DumpCounters() const override
{
}
