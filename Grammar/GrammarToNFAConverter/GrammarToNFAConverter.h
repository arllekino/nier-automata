#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>

#include "../GrammarRule/GrammarRule.h"
#include "../../NKAtoDKA/CDKA/CDKA.h"
#include "../../NKAtoDKA/CNKA/CNKA.h"
#include "../../NKAtoDKA/NKAtoDKAConverter/NKAtoDKAConverter.h"

class GrammarToNFAConverter
{
public:
    void ParseGrammar(const std::string& input)
    {
        std::istringstream iss(input);
        std::string line;

        while (std::getline(iss, line))
        {
            if (line.empty()) continue;

            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty()) continue;

            size_t arrowPos = line.find("->");
            if (arrowPos == std::string::npos)
            {
                std::cerr << "Invalid grammar rule: " << line << std::endl;
                continue;
            }

            GrammarRule rule;
            rule.nonTerminal = line.substr(0, arrowPos);
            rule.nonTerminal.erase(0, rule.nonTerminal.find_first_not_of(" \t"));
            rule.nonTerminal.erase(rule.nonTerminal.find_last_not_of(" \t") + 1);

            m_nonTerminals.insert(rule.nonTerminal);

            if (m_startSymbol.empty())
            {
                m_startSymbol = rule.nonTerminal;
            }

            std::string rightPart = line.substr(arrowPos + 2);
            rightPart.erase(0, rightPart.find_first_not_of(" \t"));
            rightPart.erase(rightPart.find_last_not_of(" \t") + 1);

            size_t start = 0;
            size_t end = rightPart.find('|');

            while (end != std::string::npos)
            {
                std::string production = rightPart.substr(start, end - start);
                production.erase(0, production.find_first_not_of(" \t"));
                production.erase(production.find_last_not_of(" \t") + 1);

                if (!production.empty())
                {
                    rule.productions.push_back(production);

                    for (char c : production)
                    {
                        if (isalpha(c) && islower(c))
                        {
                            m_terminals.insert(std::string(1, c));
                        }
                    }
                }

                start = end + 1;
                end = rightPart.find('|', start);
            }

            std::string lastProduction = rightPart.substr(start);
            lastProduction.erase(0, lastProduction.find_first_not_of(" \t"));
            lastProduction.erase(lastProduction.find_last_not_of(" \t") + 1);

            if (!lastProduction.empty())
            {
                rule.productions.push_back(lastProduction);

                for (char c : lastProduction)
                {
                    if (isalpha(c) && islower(c))
                    {
                        m_terminals.insert(std::string(1, c));
                    }
                }
            }

            m_rules.push_back(rule);
        }
    }

    void ConvertToNFA(const std::string& outputFilename) const
    {
        std::ofstream file(outputFilename);
        file << "digraph NFA {" << std::endl;

        file << "  start -> " << m_startSymbol << "0;" << std::endl;

        std::set<std::string> states;
        std::set<std::string> finalStates;
        std::vector<std::tuple<std::string, std::string, std::string>> transitions;

        int stateCounter = 0;
        std::map<std::string, int> stateIndices;

        for (const auto& nt : m_nonTerminals)
        {
            stateIndices[nt] = stateCounter;
            states.insert(nt + "0");
            stateCounter++;
        }

        std::string finalState = "F";
        states.insert(finalState);
        finalStates.insert(finalState);

        for (const auto& rule : m_rules) {
            std::string fromState = rule.nonTerminal + "0";

            for (const auto& production : rule.productions)
            {
                if (production.length() == 1)
                {
                    transitions.emplace_back(fromState, finalState, production);
                }
                else if (production.length() == 2)
                {
                    auto terminal = std::string(1, production[0]);
                    auto nonTerminal = std::string(1, production[1]);
                    std::string toState = nonTerminal + "0";
                    transitions.emplace_back(fromState, toState, terminal);
                }
            }
        }

        for (const auto& state : states)
        {
            if (finalStates.contains(state))
            {
                file << "  " << state << " [shape=doublecircle];" << std::endl;
            }
            else
            {
                file << "  " << state << " [shape=circle];" << std::endl;
            }
        }

        file << std::endl;

        for (const auto& transition : transitions)
        {
            file << "  " << std::get<0>(transition) << " -> " << std::get<1>(transition)
                 << " [label=\"" << std::get<2>(transition) << "\"];" << std::endl;
        }

        file << "}" << std::endl;
        file.close();
    }

    void ConvertToDFA(const std::string& outputFilename) const
    {
        ConvertToNFA("temp_nfa.dot");

        CNKA nfa("temp_nfa.dot");
        CDKA dfa = NKAtoDKAConverter::Convert(nfa);
        dfa.WriteToFile(outputFilename);

        remove("temp_nfa.dot");
    }
private:
    std::vector<GrammarRule> m_rules;
    std::set<std::string> m_nonTerminals;
    std::set<std::string> m_terminals;
    std::string m_startSymbol;
};
