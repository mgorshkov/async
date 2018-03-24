#include "batchcommandprocessor.h"

BatchCommandProcessor::BatchCommandProcessor(const std::string& aName, std::size_t aBulkSize, const CommandProcessors& aDependentCommandProcessors)
    : CommandProcessor(aName, aDependentCommandProcessors)
    , mBulkSize(aBulkSize)
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

void BatchCommandProcessor::ProcessCommand(const Command& aCommand)
{
    mCommandBatch.push_back(aCommand);

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
