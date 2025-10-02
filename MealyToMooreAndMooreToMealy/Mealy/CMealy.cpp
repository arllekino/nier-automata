#include "CMealy.h"
#include <fstream>
#include <iostream>
#include <sstream>

CMealy::CMealy(const std::string& filename)
{
    ParseFromFile(filename);
}

void CMealy::ParseFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find("->") != std::string::npos && line.find("label") != std::string::npos)
        {
            MealyTransition t;

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
            std::string label = line.substr(labelStart, labelEnd - labelStart);

            size_t slashPos = label.find('/');
            if (slashPos != std::string::npos)
            {
                t.input = label.substr(0, slashPos);
                t.output = label.substr(slashPos + 1);
            }
            else
            {
                t.input = label;
                t.output = "";
            }

            m_transitions.push_back(t);
            m_states.insert(t.from);
            m_states.insert(t.to);
        }
    }
}

void CMealy::AddTransition(const std::string& from, const std::string& to, 
                          const std::string& input, const std::string& output)
{
    MealyTransition t{from, to, input, output};
    m_transitions.push_back(t);
    m_states.insert(from);
    m_states.insert(to);
}

void CMealy::WriteToFile(const std::string& filename)
{
    std::ofstream file(filename);
    file << "digraph MealyMachine {" << std::endl;
    
    for (const auto& state : m_states)
    {
        file << "  " << state << " [label=\"" << state << "\"];" << std::endl;
    }
    
    file << std::endl;
    
    for (const auto& t : m_transitions)
    {
        file << "  " << t.from << " -> " << t.to 
             << " [label=\"" << t.input << "/" << t.output << "\"];" << std::endl;
    }
    
    file << "}" << std::endl;
}