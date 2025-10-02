#pragma once
#include "../Mealy/CMealy.h"
#include "../Moore/CMoore.h"

class Converter
{
public:
    static CMoore MealyToMoore(const CMealy& mealy);
    static CMealy MooreToMealy(const CMoore& moore);

private:
    static std::string DetermineStateOutput(
        const std::string& state,
        const std::vector<MealyTransition>& transitions);
};