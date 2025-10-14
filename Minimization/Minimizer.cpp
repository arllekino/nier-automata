#include "Minimizer.h"

CMealy Minimizer::MinimizeMealy(const CMealy& mealy)
{
    auto inputs = GetInputAlphabet(mealy);
    auto partition = InitialPartitionMealy(mealy, inputs);
    auto transitionTable = BuildMealyTransitionTable(mealy, inputs);

    std::vector<std::set<std::string>> newPartition;

    do
    {
        newPartition = partition;
        partition = RefinePartition(partition, inputs, transitionTable);
    }
    while (newPartition.size() != partition.size());

    CMealy minimized;

    std::map<std::string, std::string> stateToPartition;
    for (const auto& part : partition)
    {
        std::string partName = GetPartitionName(part);
        for (const auto& state : part)
        {
            stateToPartition[state] = partName;
        }
    }

    std::set<std::string> processedTransitions;
    for (const auto& transition : mealy.GetTransitions())
    {
        std::string fromPart = stateToPartition[transition.from];
        std::string toPart = stateToPartition[transition.to];

        std::string transitionKey = fromPart + "|" + transition.input + "|" + toPart + "|" + transition.output;
        if (!processedTransitions.contains(transitionKey))
        {
            minimized.AddTransition(fromPart, toPart, transition.input, transition.output);
            processedTransitions.insert(transitionKey);
        }
    }

    return minimized;
}

CMoore Minimizer::MinimizeMoore(const CMoore& moore)
{
    auto inputs = GetInputAlphabet(moore);
    auto partition = InitialPartitionMoore(moore);
    auto transitionTable = BuildMooreTransitionTable(moore, inputs);

    std::vector<std::set<std::string>> newPartition;

    do
    {
        newPartition = partition;
        partition = RefinePartitionMoore(partition, inputs, transitionTable);
    }
    while (newPartition.size() != partition.size());

    CMoore minimized;

    std::map<std::string, std::string> stateToPartition;
    for (const auto& part : partition)
    {
        std::string partName = GetPartitionName(part);

        std::string output = moore.GetStateOutputs().at(*part.begin());
        minimized.AddState(partName, output);

        for (const auto& state : part)
        {
            stateToPartition[state] = partName;
        }
    }

    std::set<std::string> processedTransitions;
    for (const auto& transition : moore.GetTransitions())
    {
        std::string fromPart = stateToPartition[transition.from];
        std::string toPart = stateToPartition[transition.to];

        std::string transitionKey = fromPart + "|" + transition.input + "|" + toPart;
        if (!processedTransitions.contains(transitionKey))
        {
            minimized.AddTransition(fromPart, toPart, transition.input);
            processedTransitions.insert(transitionKey);
        }
    }

    return minimized;
}

std::set<std::string> Minimizer::GetInputAlphabet(const CMealy& mealy)
{
    std::set<std::string> inputs;
    for (const auto& transition : mealy.GetTransitions())
    {
        inputs.insert(transition.input);
    }
    return inputs;
}

std::set<std::string> Minimizer::GetInputAlphabet(const CMoore& moore)
{
    std::set<std::string> inputs;
    for (const auto& transition : moore.GetTransitions())
    {
        inputs.insert(transition.input);
    }
    return inputs;
}

std::vector<std::set<std::string>> Minimizer::InitialPartitionMealy(
    const CMealy& mealy,
    const std::set<std::string>& inputs)
{
    auto transitionTable = BuildMealyTransitionTable(mealy, inputs);
    std::map<std::string, std::set<std::string>> outputGroups;

    for (const auto& state : mealy.GetStates())
    {
        std::string signature;
        for (const auto& input : inputs)
        {
            if (transitionTable[state].contains(input) && !transitionTable[state][input].second.empty())
            {
                signature += input + ":" + transitionTable[state][input].second + "|";
            }
            else
            {
                signature += input + ":ε|";
            }
        }
        outputGroups[signature].insert(state);
    }

    std::vector<std::set<std::string>> partition;
    partition.reserve(outputGroups.size());
    for (const auto& group : outputGroups)
    {
        partition.push_back(group.second);
    }

    return partition;
}

std::vector<std::set<std::string>> Minimizer::InitialPartitionMoore(const CMoore& moore)
{
    std::map<std::string, std::set<std::string>> outputGroups;

    for (const auto& state : moore.GetStates())
    {
        std::string output = moore.GetStateOutputs().at(state);
        outputGroups[output].insert(state);
    }

    std::vector<std::set<std::string>> partition;
    partition.reserve(outputGroups.size());
    for (const auto& group : outputGroups)
    {
        partition.push_back(group.second);
    }

    return partition;
}

std::vector<std::set<std::string>> Minimizer::RefinePartition(
    const std::vector<std::set<std::string>>& partition,
    const std::set<std::string>& inputs,
    const MealyTransitionTable& mealyTable)
{
    std::vector<std::set<std::string>> newPartition;

    for (const auto& part : partition)
    {
        if (part.size() == 1)
        {
            newPartition.push_back(part);
            continue;
        }

        std::map<std::string, std::set<std::string>> subgroups;

        for (const auto& state : part)
        {
            std::string signature;
            for (const auto& input : inputs)
            {
                if (mealyTable.at(state).contains(input) && !mealyTable.at(state).at(input).first.empty())
                {
                    const auto& nextState = mealyTable.at(state).at(input).first;
                    bool found = false;
                    for (size_t i = 0; i < partition.size(); ++i)
                    {
                        if (partition[i].contains(nextState))
                        {
                            signature += std::to_string(i) + "|";
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        signature += "NONE|";
                    }
                }
                else
                {
                    signature += "NULL|";
                }
            }
            subgroups[signature].insert(state);
        }

        for (const auto& subgroup : subgroups)
        {
            newPartition.push_back(subgroup.second);
        }
    }

    return newPartition;
}

std::vector<std::set<std::string>> Minimizer::RefinePartitionMoore(
    const std::vector<std::set<std::string>>& partition,
    const std::set<std::string>& inputs,
    const MooreTransitionTable& mooreTable)
{
    std::vector<std::set<std::string>> newPartition;

    for (const auto& part : partition)
    {
        if (part.size() == 1) {
            newPartition.push_back(part);
            continue;
        }

        std::map<std::string, std::set<std::string>> subgroups;

        for (const auto& state : part)
        {
            std::string signature;
            for (const auto& input : inputs)
            {
                if (mooreTable.at(state).contains(input) && !mooreTable.at(state).at(input).empty())
                {
                    const std::string& nextState = mooreTable.at(state).at(input);
                    bool found = false;
                    for (size_t i = 0; i < partition.size(); ++i)
                    {
                        if (partition[i].contains(nextState))
                        {
                            signature += std::to_string(i) + "|";
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        signature += "NONE|";
                    }
                }
                else
                {
                    signature += "NULL|";
                }
            }
            subgroups[signature].insert(state);
        }

        for (const auto& subgroup : subgroups)
        {
            newPartition.push_back(subgroup.second);
        }
    }

    return newPartition;
}

MealyTransitionTable Minimizer::BuildMealyTransitionTable(
    const CMealy& mealy,
    const std::set<std::string>& inputs)
{
    MealyTransitionTable table;

    for (const auto& state : mealy.GetStates())
    {
        for (const auto& input : inputs)
        {
            table[state][input] = {"", ""};
        }
    }

    for (const auto& transition : mealy.GetTransitions())
    {
        table[transition.from][transition.input] = {transition.to, transition.output};
    }

    return table;
}

MooreTransitionTable Minimizer::BuildMooreTransitionTable(
    const CMoore& moore,
    const std::set<std::string>& inputs)
{
    MooreTransitionTable table;

    for (const auto& state : moore.GetStates())
    {
        for (const auto& input : inputs)
        {
            table[state][input] = "";
        }
    }

    for (const auto& transition : moore.GetTransitions())
    {
        table[transition.from][transition.input] = transition.to;
    }

    return table;
}

std::string Minimizer::GetPartitionName(const std::set<std::string>& states)
{
    if (states.size() == 1)
    {
        return *states.begin();
    }

    std::string name = "S{";
    for (const auto& state : states)
    {
        name += state + ",";
    }
    if (!states.empty())
    {
        name.pop_back();
    }
    name += "}";
    return name;
}