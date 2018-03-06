#include <iostream>
#include <sstream>
#include <fstream>

#ifdef EXPERIMENT_PART1
#include <numeric>
#include <algorithm>
#endif

#include "reportwriter.h"
#include "utils.h"

ReportWriter::ReportWriter(const std::string& aName)
    : CommandProcessor(aName)
{
}

void ReportWriter::ProcessBatch(const CommandBatch& commandBatch)
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
    std::ostringstream filename;
    filename << mName << Separator << "bulk" << Separator
        << mCounters.mBlockCounter << Separator << seconds << Separator << this << ".log";
    return filename.str();
}

