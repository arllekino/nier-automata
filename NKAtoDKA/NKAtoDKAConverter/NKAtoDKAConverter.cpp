#include "NKAtoDKAConverter.h"
#include <sstream>
#include <queue>

std::set<std::string> NKAtoDKAConverter::GetEpsilonClosure(const CNKA& nfa, const std::set<std::string>& states)
{
    std::set<std::string> closure = states;
    std::queue<std::string> queue;
    std::set<std::string> visited;

    for (const auto& state : states)
    {
        queue.push(state);
        visited.insert(state);
    }

    while (!queue.empty())
    {
        std::string current = queue.front();
        queue.pop();

        for (const auto& transition : nfa.GetTransitions())
        {
            if (transition.from == current && transition.input == "ε")
            {
                if (!visited.contains(transition.to))
                {
                    closure.insert(transition.to);
                    queue.push(transition.to);
                    visited.insert(transition.to);
                }
            }
        }
    }

    return closure;
}

std::set<std::string> NKAtoDKAConverter::Move(
    const CNKA& nfa,
    const std::set<std::string>& states,
    const std::string& symbol)
{
    std::set<std::string> result;

    for (const auto& state : states)
    {
        for (const auto& transition : nfa.GetTransitions())
        {
            if (transition.from == state && transition.input == symbol)
            {
                result.insert(transition.to);
            }
        }
    }

    return result;
}

std::string NKAtoDKAConverter::GetStateName(const std::set<std::string>& states)
{
    if (states.empty()) return "-";

    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& state : states) {
        if (!first) ss << ",";
        ss << state;
        first = false;
    }
    ss << "}";
    return ss.str();
}

CDKA NKAtoDKAConverter::Convert(const CNKA& nfa)
{
    CDKA dfa;
    std::set<std::string> alphabet = nfa.GetAlphabet();

    std::set<std::string> startStateSet = GetEpsilonClosure(nfa, {nfa.GetInitialState()});
    std::string startStateName = GetStateName(startStateSet);

    dfa.SetInitialState(startStateName);
    dfa.AddState(startStateName);

    for (const auto& state : startStateSet)
    {
        if (nfa.GetFinalStates().contains(state))
        {
            dfa.AddFinalState(startStateName);
            break;
        }
    }

    std::queue<std::set<std::string>> unprocessedStates;
    std::map<std::set<std::string>, std::string> processedStates;

    unprocessedStates.push(startStateSet);
    processedStates[startStateSet] = startStateName;

    std::string deadStateName = "-";
    bool deadStateUsed = false;

    while (!unprocessedStates.empty())
    {
        std::set<std::string> currentStateSet = unprocessedStates.front();
        unprocessedStates.pop();
        std::string currentStateName = processedStates[currentStateSet];

        for (const auto& symbol : alphabet)
        {
            if (symbol == "ε") continue;

            std::set<std::string> moved = Move(nfa, currentStateSet, symbol);
            std::set<std::string> nextStateSet = GetEpsilonClosure(nfa, moved);

            if (nextStateSet.empty())
            {
                if (!deadStateUsed)
                {
                    dfa.AddState(deadStateName);
                    for (const auto& sym : alphabet)
                    {
                        if (sym != "ε")
                        {
                            dfa.AddTransition(deadStateName, deadStateName, sym);
                        }
                    }
                    deadStateUsed = true;
                }
                dfa.AddTransition(currentStateName, deadStateName, symbol);
            }
            else
            {
                std::string nextStateName = GetStateName(nextStateSet);

                if (!processedStates.contains(nextStateSet))
                {
                    processedStates[nextStateSet] = nextStateName;
                    unprocessedStates.push(nextStateSet);
                    dfa.AddState(nextStateName);

                    for (const auto& state : nextStateSet)
                    {
                        if (nfa.GetFinalStates().contains(state))
                        {
                            dfa.AddFinalState(nextStateName);
                            break;
                        }
                    }
                }

                dfa.AddTransition(currentStateName, nextStateName, symbol);
            }
        }
    }
    dfa.FilterTransitions();

    return dfa;
}