#include <string>
#include <vector>
#include <sstream>

#include "utils.h"

static std::string Join(const std::vector<Command>& v)
{
    std::stringstream ss;
    for(size_t i = 0; i < v.size(); ++i)
    {
        if(i != 0)
            ss << ", ";
        ss << v[i].mText;
    }
    return ss.str();
}

Command MakeCommandFromBatch(const CommandBatch& aCommandBatch)
{
    std::string output = "bulk: " + Join(aCommandBatch.mCommands);
    Command command{output, aCommandBatch.mTimestamp};
    return command;
}
