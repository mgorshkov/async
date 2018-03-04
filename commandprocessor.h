#pragma once

#include <vector>
#include <memory>
#include <string>

#include "structs.h"
#include "icommandprocessor.h"

using CommandProcessors = std::vector<std::shared_ptr<ICommandProcessor>>;

class CommandProcessor : public ICommandProcessor
{
public:
    CommandProcessor(const std::string& aName, const CommandProcessors& dependentCommandProcessors = CommandProcessors());

    void StartBlock() override;
    void FinishBlock() override;

    void ProcessLine(const std::string& line) override;

    void ProcessCommand(const Command& command) override;

    void ProcessBatch(const CommandBatch& commandBatch) override;

    void Stop() override;

    void DumpCounters() const override;

protected:
    std::string mName;
    Counters mCounters;

private:
    CommandProcessors mDependentCommandProcessors;
};
