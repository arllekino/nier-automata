#include "CMoore.h"
#include <fstream>
#include <iostream>
#include <sstream>

CMoore::CMoore(const std::string& filename)
{
    ParseFromFile(filename);
}

void CMoore::ParseFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find("[label") != std::string::npos && line.find('/') != std::string::npos)
        {
            size_t bracketPos = line.find('[');
            std::string state = line.substr(0, bracketPos);
            state.erase(0, state.find_first_not_of(" \t"));
            state.erase(state.find_last_not_of(" \t") + 1);

            size_t labelStart = line.find('\"') + 1;
            size_t labelEnd = line.find('\"', labelStart);
            std::string label = line.substr(labelStart, labelEnd - labelStart);

            size_t slashPos = label.find('/');
            if (slashPos != std::string::npos)
            {
                std::string output = label.substr(slashPos + 1);
                m_stateOutputs[state] = output;
                m_states.insert(state);
            }
        }
        else if (line.find("->") != std::string::npos && line.find("label") != std::string::npos)
        {
            MooreTransition t;

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

            m_transitions.push_back(t);
        }
    }
}

void CMoore::AddState(const std::string& state, const std::string& output)
{
    m_states.insert(state);
    m_stateOutputs[state] = output;
}

void CMoore::AddTransition(const std::string& from, const std::string& to, const std::string& input)
{
    MooreTransition t{from, to, input};
    m_transitions.push_back(t);
}

void CMoore::WriteToFile(const std::string& filename)
{
    std::ofstream file(filename);
    file << "digraph MooreMachine {" << std::endl;
    
    for (const auto& state : m_states)
    {
        std::string output = m_stateOutputs[state];
        file << "  " << state << " [label=\"" << state << " / " << output << "\"];" << std::endl;
    }
    
    file << std::endl;
    
    for (const auto& t : m_transitions)
    {
        file << "  " << t.from << " -> " << t.to 
             << " [label=\"" << t.input << "\"];" << std::endl;
    }
    
    file << "}" << std::endl;
}