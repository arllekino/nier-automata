#pragma once
#include <fstream>
#include <string>

class IMachine
{
public:
    virtual void WriteToFile(const std::string& filename) = 0;
    virtual ~IMachine() = default;
};