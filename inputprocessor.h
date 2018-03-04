#pragma once

#include <string>
#include "commandprocessor.h"

class InputProcessor : public CommandProcessor
{
public:
    InputProcessor(const CommandProcessors& dependentCommandProcessors);

    void ProcessLine(const std::string& line) override;

    void DumpCounters() const override;

private:
    int mBlockDepth{0};
};
