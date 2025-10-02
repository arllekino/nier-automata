#include "Converter.h"

CMoore Converter::MealyToMoore(const CMealy& mealy)
{
    CMoore moore;
    const auto& transitions = mealy.GetTransitions();
    const auto& states = mealy.GetStates();
    
    std::map<std::string, std::string> stateOutputs;
    
    for (const auto& state : states)
    {
        std::string output = DetermineStateOutput(state, transitions);
        moore.AddState(state, output);
    }
    
    for (const auto& t : transitions)
    {
        moore.AddTransition(t.from, t.to, t.input);
    }
    
    return moore;
}

CMealy Converter::MooreToMealy(const CMoore& moore)
{
    CMealy mealy;
    const auto& transitions = moore.GetTransitions();
    const auto& stateOutputs = moore.GetStateOutputs();
    
    for (const auto& t : transitions)
    {
        const std::string& output = stateOutputs.at(t.to);
        mealy.AddTransition(t.from, t.to, t.input, output);
    }
    
    return mealy;
}

std::string Converter::DetermineStateOutput(
    const std::string& state,
    const std::vector<MealyTransition>& transitions)
{
    for (const auto& t : transitions)
    {
        if (t.to == state)
        {
            return t.output;
        }
    }
    
    for (const auto& t : transitions)
    {
        if (t.from == state)
        {
            return t.output;
        }
    }
    
    return "";
}