#pragma once

#include <vector>

#include "structs.h"
#include "commandprocessor.h"

class BatchCommandProcessor : public CommandProcessor
{
public:
    BatchCommandProcessor(const std::string& aName, std::size_t aBulkSize, const CommandProcessors& aDependentCommandProcessors);

    void StartBlock() override;
    void FinishBlock() override;

    void ProcessCommand(const Command& aCommand) override;
    void Stop() override;

private:
    void ClearBatch();
    void DumpBatch();

    std::size_t mBulkSize;
    bool mBlockForced;
    std::vector<Command> mCommandBatch;
};
