#pragma once
#include <set>
#include <map>

#include "../CDKA/CDKA.h"
#include "../CNKA/CNKA.h"

class NKAtoDKAConverter
{
public:
    static CDKA Convert(const CNKA& nfa);

private:
    static std::set<std::string> GetEpsilonClosure(const CNKA& nfa, const std::set<std::string>& states);
    static std::set<std::string> Move(
        const CNKA& nfa,
        const std::set<std::string>& states,
        const std::string& symbol);
    static std::string GetStateName(const std::set<std::string>& states);
};