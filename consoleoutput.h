#pragma once

#include <string>

#include "commandprocessor.h"

class ConsoleOutput : public CommandProcessor
{
public:
    ConsoleOutput(const std::string& aName);
    ~ConsoleOutput();

    void ProcessBatch(const CommandBatch& aCommandBatch) override;
};
