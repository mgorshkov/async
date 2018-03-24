#pragma once

class ICommandProcessor
{
public:
    virtual ~ICommandProcessor() = default;

    virtual void StartBlock() = 0;
    virtual void FinishBlock() = 0;

    virtual void ProcessLine(const std::string&) = 0;
    virtual void ProcessCommand(const Command&) = 0;
    virtual void ProcessBatch(const CommandBatch&) = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual void DumpCounters() const = 0;
};

