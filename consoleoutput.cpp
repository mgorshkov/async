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

void ConsoleOutput::ProcessBatch(const CommandBatch& commandBatch)
{
    Command command = MakeCommandFromBatch(commandBatch);

    std::cout << command.mText << std::endl;

    CommandProcessor::ProcessBatch(commandBatch);
}
