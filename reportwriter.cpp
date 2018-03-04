#include "reportwriter.h"

ReportWriter::ReportWriter(const std::string& aName)
    : CommandProcessor(aName)
{
}

void ReportWriter::ProcessBatch(const CommandBatch& commandBatch) override
{
    Command command = MakeCommandFromBatch(commandBatch);

    std::ofstream file(GetFilename(command), std::ofstream::out);
    file << command.mText << std::endl;

    CommandProcessor::ProcessBatch(commandBatch);

#ifdef EXPERIMENT_PART1
    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 0);
    for (int i = 0; i < 1000000; ++i)
        std::random_shuffle(v.begin(), v.end());
#endif
}

std::string ReportWriter::GetFilename(const Command& command)
{
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
    command.mTimestamp.time_since_epoch()).count();
    std::stringstream filename;
    filename << mName << Separator << "bulk" << Separator
        << mCounters.mBlockCounter << Separator << seconds << ".log";
    return filename.str();
}

