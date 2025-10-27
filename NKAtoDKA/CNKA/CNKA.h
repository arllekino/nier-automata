#pragma once
#include <vector>
#include <set>
#include <string>

#include "../../MealyToMooreAndMooreToMealy/IMachine/IMachine.h"

struct NFATransition
{
    std::string from;
    std::string to;
    std::string input;
};

class CNKA : public IMachine
{
public:
    CNKA() = default;
    explicit CNKA(const std::string& filename);

    void WriteToFile(const std::string& filename) override;

    [[nodiscard]] const std::vector<NFATransition>& GetTransitions() const { return m_transitions; }
    [[nodiscard]] const std::set<std::string>& GetStates() const { return m_states; }
    [[nodiscard]] const std::set<std::string>& GetAlphabet() const { return m_alphabet; }
    [[nodiscard]] const std::set<std::string>& GetFinalStates() const { return m_finalStates; }
    [[nodiscard]] const std::string& GetInitialState() const { return m_initialState; }

    void AddTransition(const std::string& from, const std::string& to, const std::string& input);
    void AddFinalState(const std::string& state) { m_finalStates.insert(state); }
    void SetInitialState(const std::string& state) { m_initialState = state; m_states.insert(state); }

private:
    std::vector<NFATransition> m_transitions;
    std::set<std::string> m_states;
    std::set<std::string> m_alphabet;
    std::set<std::string> m_finalStates;
    std::string m_initialState;

    void ParseFromFile(const std::string& filename);
};