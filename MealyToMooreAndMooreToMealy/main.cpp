#include <iostream>
#include "Mealy/CMealy.h"
#include "Moore/CMoore.h"
#include "Converter/Converter.h"

int main()
{
    try
    {
        const CMealy mealy("/Users/miroslav.aktuganov/CLionProjects/avtomats/MealyToMooreAndMooreToMealy/input.dot");
        CMoore moore = Converter::MealyToMoore(mealy);
        moore.WriteToFile("/Users/miroslav.aktuganov/CLionProjects/avtomats/MealyToMooreAndMooreToMealy/MooreToMealy.dot");

        CMealy mealyFromMoore = Converter::MooreToMealy(moore);
        mealyFromMoore.WriteToFile("/Users/miroslav.aktuganov/CLionProjects/avtomats/MealyToMooreAndMooreToMealy/MealyToMoore.dot");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}