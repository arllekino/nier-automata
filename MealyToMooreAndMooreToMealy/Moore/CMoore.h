#pragma once
#include "../IMachine/IMachine.h"
#include <vector>
#include <map>
#include <set>

struct MooreTransition
{
    std::string from;
    std::string to;
    std::string input;
};

class CMoore : public IMachine
{
public:
    CMoore() = default;
    explicit CMoore(const std::string& filename);

    void WriteToFile(const std::string& filename) override;

    [[nodiscard]] const std::vector<MooreTransition>& GetTransitions() const { return m_transitions; }
    [[nodiscard]] const std::map<std::string, std::string>& GetStateOutputs() const { return m_stateOutputs; }
    [[nodiscard]] const std::set<std::string>& GetStates() const { return m_states; }
    
    void AddState(const std::string& state, const std::string& output);
    void AddTransition(const std::string& from, const std::string& to, const std::string& input);

private:
    std::vector<MooreTransition> m_transitions;
    std::map<std::string, std::string> m_stateOutputs;
    std::set<std::string> m_states;
    
    void ParseFromFile(const std::string& filename);
};