#pragma once
#include "../IMachine/IMachine.h"
#include <vector>
#include <map>
#include <set>

struct MealyTransition
{
    std::string from;
    std::string to;
    std::string input;
    std::string output;
};

class CMealy : public IMachine
{
public:
    CMealy() = default;
    explicit CMealy(const std::string& filename);

    void WriteToFile(const std::string& filename) override;
    
    [[nodiscard]] const std::vector<MealyTransition>& GetTransitions() const { return m_transitions; }
    [[nodiscard]] const std::set<std::string>& GetStates() const { return m_states; }
    
    void AddTransition(const std::string& from, const std::string& to,
                      const std::string& input, const std::string& output);

private:
    std::vector<MealyTransition> m_transitions;
    std::set<std::string> m_states;
    
    void ParseFromFile(const std::string& filename);
};