#include <iostream>
#include "Minimizer.h"

int main(int argc, char* argv[])
{
    if (argc != 4 || std::strcmp(argv[3], "-mealy") != 0 && std::strcmp(argv[3], "-moore") != 0)
    {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file> -mealy|-moore" << std::endl;
        return 1;
    }

    auto inputFilename = std::string(argv[1]);
    auto outputFilename = std::string(argv[2]);

    try
    {
        if (std::strcmp(argv[3], "-mealy") == 0)
        {
            const CMealy mealy(inputFilename);
            CMealy minimizedMealy = Minimizer::MinimizeMealy(mealy);
            minimizedMealy.WriteToFile(outputFilename);
        }
        if (std::strcmp(argv[3], "-moore") == 0)
        {
            const CMoore moore(inputFilename);
            CMoore minimizedMoore = Minimizer::MinimizeMoore(moore);
            minimizedMoore.WriteToFile(outputFilename);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}