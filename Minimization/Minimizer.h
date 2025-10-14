#pragma once
#include <map>
#include <set>
#include <vector>

#include "../MealyToMooreAndMooreToMealy/Mealy/CMealy.h"
#include "../MealyToMooreAndMooreToMealy/Moore/CMoore.h"

using MealyTransitionTable = std::map<std::string, std::map<std::string, std::pair<std::string, std::string>>>;
using MooreTransitionTable = std::map<std::string, std::map<std::string, std::string>>;

class Minimizer
{
public:
    static CMealy MinimizeMealy(const CMealy& mealy);
    static CMoore MinimizeMoore(const CMoore& moore);

private:
    static std::set<std::string> GetInputAlphabet(const CMealy& mealy);
    static std::set<std::string> GetInputAlphabet(const CMoore& moore);

    static std::vector<std::set<std::string>> InitialPartitionMealy(
        const CMealy& mealy,
        const std::set<std::string>& inputs);

    static std::vector<std::set<std::string>> InitialPartitionMoore(const CMoore& moore);

    static std::vector<std::set<std::string>> RefinePartition(
        const std::vector<std::set<std::string>>& partition,
        const std::set<std::string>& inputs,
        const MealyTransitionTable& mealyTable);

    static std::vector<std::set<std::string>> RefinePartitionMoore(
        const std::vector<std::set<std::string>>& partition,
        const std::set<std::string>& inputs,
        const MooreTransitionTable& mooreTable);

    static MealyTransitionTable BuildMealyTransitionTable(
        const CMealy& mealy,
        const std::set<std::string>& inputs);

    static MooreTransitionTable BuildMooreTransitionTable(
        const CMoore& moore,
        const std::set<std::string>& inputs);

    static std::string GetPartitionName(const std::set<std::string>& states);
};
