#include "CDKA.h"
#include <fstream>

void CDKA::AddTransition(const std::string& from, const std::string& to, const std::string& input)
{
    const DFATransition t{from, to, input};
    m_transitions.push_back(t);
    m_states.insert(from);
    m_states.insert(to);
}

void CDKA::FilterTransitions()
{
    std::erase_if(m_transitions,
        [](const auto& transition) {
            return transition.from == "-";
        });

    std::erase_if(m_states,
        [](const auto& state) {
            return state == "-";
        });
}

void CDKA::WriteToFile(const std::string& filename)
{
    std::ofstream file(filename);
    file << "digraph DFA {" << std::endl;

    file << "  start -> " << m_initialState << ";" << std::endl;

    for (const auto& state : m_states)
    {
        if (m_finalStates.count(state))
        {
            file << "  " << state << " [shape=doublecircle];" << std::endl;
        }
        else
        {
            file << "  " << state << " [shape=circle];" << std::endl;
        }
    }

    file << std::endl;

    for (const auto& t : m_transitions)
    {
        file << "  " << t.from << " -> " << t.to
             << " [label=\"" << t.input << "\"];" << std::endl;
    }

    file << "}" << std::endl;
    file.close();
}