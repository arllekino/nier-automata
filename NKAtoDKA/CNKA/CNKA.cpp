#include "CNKA.h"
#include <fstream>
#include <iostream>
#include <sstream>

CNKA::CNKA(const std::string& filename)
{
    ParseFromFile(filename);
}

void CNKA::ParseFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line.find("//") == 0 || line.find("digraph") != std::string::npos ||
            line.find('}') != std::string::npos)
        {
            continue;
        }

        if (line.find("->") != std::string::npos && line.find("[label=") == std::string::npos)
        {
            size_t arrowPos = line.find("->");
            std::string from = line.substr(0, arrowPos);
            from.erase(0, from.find_first_not_of(" \t"));
            from.erase(from.find_last_not_of(" \t") + 1);

            if (from == "start")
            {
                std::string to = line.substr(arrowPos + 2);
                to.erase(0, to.find_first_not_of(" \t"));
                to.erase(to.find_last_not_of(" \t") + 1);
                if (to.back() == ';') to.pop_back();
                m_initialState = to;
                m_states.insert(to);
            }
            continue;
        }

        if (line.find("[label=") != std::string::npos && line.find("->") == std::string::npos)
        {
            size_t bracketPos = line.find('[');
            std::string state = line.substr(0, bracketPos);
            state.erase(0, state.find_first_not_of(" \t"));
            state.erase(state.find_last_not_of(" \t") + 1);
            m_states.insert(state);

            if (line.find("doublecircle") != std::string::npos)
            {
                m_finalStates.insert(state);
            }

            continue;
        }

        if (line.find("->") != std::string::npos && line.find("[label=") != std::string::npos)
        {
            NFATransition t;

            size_t arrowPos = line.find("->");
            t.from = line.substr(0, arrowPos);
            t.from.erase(0, t.from.find_first_not_of(" \t"));
            t.from.erase(t.from.find_last_not_of(" \t") + 1);

            size_t bracketPos = line.find('[');
            t.to = line.substr(arrowPos + 2, bracketPos - arrowPos - 2);
            t.to.erase(0, t.to.find_first_not_of(" \t"));
            t.to.erase(t.to.find_last_not_of(" \t") + 1);

            size_t labelStart = line.find('\"') + 1;
            size_t labelEnd = line.find('\"', labelStart);
            t.input = line.substr(labelStart, labelEnd - labelStart);

            if (t.input == "ε" || t.input == "epsilon" || t.input == "E")
            {
                t.input = "ε";
            }
            else
            {
                m_alphabet.insert(t.input);
            }

            m_transitions.push_back(t);
            m_states.insert(t.from);
            m_states.insert(t.to);
        }
    }

    if (m_initialState.empty() && !m_states.empty())
    {
        m_initialState = *m_states.begin();
    }
}

void CNKA::AddTransition(const std::string& from, const std::string& to, const std::string& input)
{
    const NFATransition t{from, to, input};
    m_transitions.push_back(t);
    m_states.insert(from);
    m_states.insert(to);

    if (input != "ε")
    {
        m_alphabet.insert(input);
    }
}

void CNKA::WriteToFile(const std::string& filename)
{
    std::ofstream file(filename);
    file << "digraph NFA {" << std::endl;

    for (const auto& state : m_states)
    {
        file << "  " << state << " [label=\"" << state << "\"];" << std::endl;
    }

    file << std::endl;

    for (const auto& t : m_transitions)
    {
        file << "  " << t.from << " -> " << t.to
             << " [label=\"" << t.input << "\"];" << std::endl;
    }

    file << "}" << std::endl;
}