#pragma once

#include <string>

#include "commandprocessor.h"

class ReportWriter : public CommandProcessor
{
public:
    ReportWriter(const std::string& aName);

    void ProcessBatch(const CommandBatch& aCommandBatch) override;

private:
    std::string GetFilename(const Command& aCommand);

    static const char Separator = '-';
};
