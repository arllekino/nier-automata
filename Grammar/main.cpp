#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "GrammarToNFAConverter/GrammarToNFAConverter.h"

void AssertArgumentsCountValid(int argc)
{
    if (argc != 3)
    {
        throw std::invalid_argument("Usage: ./grammar input_file output_file\nWhere input_file format: A->aB|bC|a");
    }
}

int main(int argc, char *argv[])
{
    try
    {
        AssertArgumentsCountValid(argc);

        std::ifstream inputFile(argv[1]);
        if (!inputFile.is_open())
        {
            throw std::runtime_error("Cannot open input file: " + std::string(argv[1]));
        }

        std::string grammar;
        std::string line;
        while (std::getline(inputFile, line))
        {
            if (line.empty()) continue;

            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (!line.empty())
            {
                grammar += line + "\n";
            }
        }
        inputFile.close();

        if (grammar.empty())
        {
            std::cerr << "Input file is empty" << std::endl;
            return 1;
        }

        GrammarToNFAConverter converter;
        converter.ParseGrammar(grammar);
        converter.ConvertToDFA(argv[2]);
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Argument error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}