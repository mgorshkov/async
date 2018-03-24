#pragma once

#include <string>
#include "commandprocessor.h"

class InputProcessor : public CommandProcessor
{
public:
    InputProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors);

    void ProcessLine(const std::string& aLine) override;

    void DumpCounters() const override;

private:
    int mBlockDepth{0};
};
