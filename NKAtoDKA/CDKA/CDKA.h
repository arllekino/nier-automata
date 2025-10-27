#pragma once
#include <vector>
#include <set>
#include <string>

#include "../../MealyToMooreAndMooreToMealy/IMachine/IMachine.h"

struct DFATransition
{
    std::string from;
    std::string to;
    std::string input;
};

class CDKA : public IMachine
{
public:
    CDKA() = default;

    void WriteToFile(const std::string& filename) override;

    [[nodiscard]] const std::vector<DFATransition>& GetTransitions() const { return m_transitions; }
    [[nodiscard]] const std::set<std::string>& GetStates() const { return m_states; }
    [[nodiscard]] const std::set<std::string>& GetFinalStates() const { return m_finalStates; }
    [[nodiscard]] const std::string& GetInitialState() const { return m_initialState; }

    void AddTransition(const std::string& from, const std::string& to, const std::string& input);
    void AddState(const std::string& state) { m_states.insert(state); }
    void AddFinalState(const std::string& state) { m_finalStates.insert(state); }
    void SetInitialState(const std::string& state) { m_initialState = state; AddState(state); }
    void FilterTransitions();

private:
    std::vector<DFATransition> m_transitions;
    std::set<std::string> m_states;
    std::set<std::string> m_finalStates;
    std::string m_initialState;
};