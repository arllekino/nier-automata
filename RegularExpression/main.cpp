#include <iostream>
#include <string>
#include <stack>
#include "../NKAtoDKA/CNKA/CNKA.h"
#include "../NKAtoDKA/CDKA/CDKA.h"
#include "../NKAtoDKA/NKAtoDKAConverter/NKAtoDKAConverter.h"

struct NFAState
{
    std::string name;
    bool isFinal = false;
};

struct SimpleNFA
{
    std::string startState;
    std::string endState;
};

SimpleNFA CreateCharNFA(const std::string& symbol, int& state_counter)
{
    SimpleNFA nfa;
    nfa.startState = std::to_string(state_counter++);
    nfa.endState = std::to_string(state_counter++);
    return nfa;
}

SimpleNFA CreateOrNFA(const SimpleNFA& nfa1, const SimpleNFA& nfa2, int& stateCounter, CNKA& resultNfa)
{
    SimpleNFA nfa;
    nfa.startState = std::to_string(stateCounter++);
    nfa.endState = std::to_string(stateCounter++);

    resultNfa.AddTransition(nfa.startState, nfa1.startState, "ε");
    resultNfa.AddTransition(nfa.startState, nfa2.startState, "ε");

    resultNfa.AddTransition(nfa1.endState, nfa.endState, "ε");
    resultNfa.AddTransition(nfa2.endState, nfa.endState, "ε");

    return nfa;
}

SimpleNFA CreateConcatNFA(const SimpleNFA& nfa1, const SimpleNFA& nfa2, CNKA& resultNfa)
{
    SimpleNFA nfa;
    nfa.startState = nfa1.startState;
    nfa.endState = nfa2.endState;

    resultNfa.AddTransition(nfa1.endState, nfa2.startState, "ε");

    return nfa;
}

SimpleNFA CreateKleeneNFA(const SimpleNFA& nfa1, int& state_counter, CNKA& resultNfa)
{
    SimpleNFA nfa;
    nfa.startState = std::to_string(state_counter++);
    nfa.endState = std::to_string(state_counter++);

    resultNfa.AddTransition(nfa.startState, nfa1.startState, "ε");
    resultNfa.AddTransition(nfa1.endState, nfa.endState, "ε");
    resultNfa.AddTransition(nfa1.endState, nfa1.startState, "ε");
    resultNfa.AddTransition(nfa.startState, nfa.endState, "ε");

    return nfa;
}

std::unique_ptr<CNKA> RegexToNFA(const std::string& regex)
{
    auto nfa = std::make_unique<CNKA>();

    if (regex.empty())
    {
        nfa->SetInitialState("0");
        nfa->AddFinalState("0");
        nfa->AddTransition("0", "0", "ε");
        return nfa;
    }

    std::stack<char> operators;
    std::stack<SimpleNFA> operands;

    int stateCounter = 0;
    bool lastWasOperand = false;

    auto applyOperator = [&]() {
        if (operators.empty()) return;

        char op = operators.top();
        operators.pop();

        if (op == '|')
        {
            if (operands.size() < 2) throw std::runtime_error("Invalid regex: not enough operands for |");
            SimpleNFA right = operands.top(); operands.pop();
            SimpleNFA left = operands.top(); operands.pop();
            operands.push(CreateOrNFA(left, right, stateCounter, *nfa));
        }
        else if (op == '.')
        {
            if (operands.size() < 2) throw std::runtime_error("Invalid regex: not enough operands for concat");
            SimpleNFA right = operands.top(); operands.pop();
            SimpleNFA left = operands.top(); operands.pop();
            operands.push(CreateConcatNFA(left, right, *nfa));
        }
        else if (op == '*')
        {
            if (operands.empty()) throw std::runtime_error("Invalid regex: no operand for *");
            SimpleNFA operand = operands.top(); operands.pop();
            operands.push(CreateKleeneNFA(operand, stateCounter, *nfa));
        }
    };

    for (size_t i = 0; i < regex.length(); ++i)
    {
        char c = regex[i];

        if (c == '(') {
            if (lastWasOperand) {
                while (!operators.empty() && operators.top() == '.') {
                    applyOperator();
                }
                operators.push('.');
            }
            operators.push(c);
            lastWasOperand = false;
        }
        else if (c == ')')
        {
            while (!operators.empty() && operators.top() != '(')
            {
                applyOperator();
            }
            if (operators.empty()) throw std::runtime_error("Unmatched parentheses");
            operators.pop();
            lastWasOperand = true;
        }
        else if (c == '|')
        {
            while (!operators.empty() && (operators.top() == '.' || operators.top() == '|'))
            {
                applyOperator();
            }
            operators.push(c);
            lastWasOperand = false;
        }
        else if (c == '*')
        {
            if (!lastWasOperand) throw std::runtime_error("Invalid use of *");
            operators.push(c);
            applyOperator();
            lastWasOperand = true;
        }
        else
        {
            if (lastWasOperand) {
                while (!operators.empty() && operators.top() == '.') {
                    applyOperator();
                }
                operators.push('.');
            }

            SimpleNFA char_nfa = CreateCharNFA(std::string(1, c), stateCounter);
            nfa->AddTransition(char_nfa.startState, char_nfa.endState, std::string(1, c));
            operands.push(char_nfa);
            lastWasOperand = true;
        }
    }

    while (!operators.empty())
    {
        applyOperator();
    }

    if (operands.size() != 1)
    {
        throw std::runtime_error("Invalid regex: could not parse completely");
    }

    SimpleNFA result = operands.top();
    nfa->SetInitialState(result.startState);
    nfa->AddFinalState(result.endState);

    return nfa;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <regex> <output_file.dot>" << std::endl;
        std::cerr << "Supported operations: |, *, ()" << std::endl;
        return 1;
    }

    std::string regex = argv[1];
    std::string outputFile = argv[2];

    try
    {
        auto nfa = RegexToNFA(regex);
        CDKA dfa = NKAtoDKAConverter::Convert(*nfa);
        dfa.WriteToFile(outputFile);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}