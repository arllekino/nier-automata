#pragma once
#include <string>
#include <vector>

struct GrammarRule
{
    std::string nonTerminal;
    std::vector<std::string> productions;
};
