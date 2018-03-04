#include "batchcommandprocessor.h"

BatchCommandProcessor::BatchCommandProcessor(size_t bulkSize, const CommandProcessors& dependentCommandProcessors)
    : CommandProcessor("main", dependentCommandProcessors)
    , mBulkSize(bulkSize)
    , mBlockForced(false)
{
}

void BatchCommandProcessor::StartBlock()
{
    mBlockForced = true;
    DumpBatch();
}

void BatchCommandProcessor::FinishBlock()
{
    mBlockForced = false;
    DumpBatch();
}

void BatchCommandProcessor::ProcessCommand(const Command& command)
{
    mCommandBatch.push_back(command);

    if (!mBlockForced && mCommandBatch.size() >= mBulkSize)
    {
        DumpBatch();
    }
}

void BatchCommandProcessor::Stop()
{
    if (!mBlockForced)
        DumpBatch();
    CommandProcessor::Stop();
}

void BatchCommandProcessor::ClearBatch()
{
    mCommandBatch.clear();
}

void BatchCommandProcessor::DumpBatch()
{
    if (mCommandBatch.empty())
        return;
    CommandBatch commandBatch{mCommandBatch, mCommandBatch[0].mTimestamp};
    CommandProcessor::ProcessBatch(commandBatch);
    ClearBatch();
}
