#include <iostream>

#include "CDKA/CDKA.h"
#include "NKAtoDKAConverter/NKAtoDKAConverter.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input NFA file> <output DFA file>" << std::endl;
        return 1;
    }

    auto inputFilename = std::string(argv[1]);
    auto outputFilename = std::string(argv[2]);

    try
    {
        CNKA nfa(inputFilename);
        CDKA dfa = NKAtoDKAConverter::Convert(nfa);
        dfa.WriteToFile(outputFilename);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}