#pragma once

#include "structs.h"
#include "commandprocessor.h"

class BatchCommandProcessor : public CommandProcessor
{
public:
    BatchCommandProcessor(std::size_t bulkSize, const CommandProcessors& dependentCommandProcessors);

    void StartBlock() override;
    void FinishBlock() override;

    void ProcessCommand(const Command& command) override;
    void Stop() override;

private:
    void ClearBatch();
    void DumpBatch();
};
