#include <iostream>

#include "consoleoutput.h"
#include "utils.h"

ConsoleOutput::ConsoleOutput(const std::string& aName)
    : CommandProcessor(aName)
{
}

ConsoleOutput::~ConsoleOutput()
{
}

void ConsoleOutput::ProcessBatch(const CommandBatch& aCommandBatch)
{
    Command command = MakeCommandFromBatch(aCommandBatch);

    std::cout << command.mText << std::endl;

    CommandProcessor::ProcessBatch(aCommandBatch);
}
