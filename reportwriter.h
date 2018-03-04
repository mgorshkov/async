#pragma once

class ReportWriter : public CommandProcessor
{
public:
    ReportWriter(const std::string& aName);

    void ProcessBatch(const CommandBatch& commandBatch) override;

private:
    std::string GetFilename(const Command& command);

    static const char Separator = '-';
};
