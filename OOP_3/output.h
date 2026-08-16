#ifndef OUTPUT_H_DEFINED
#define OUTPUT_H_DEFINED

#include "main.h"
#include "print.h"

#include <stdexcept> // runtime_error

// -------------------------------------------------------
// Template: outputStudentai
// Išveda studentus į konsolę arba į failą, priklausomai nuo vartotojo pasirinkimo.
// -------------------------------------------------------
template<typename Container>
void outputStudentai(const Container& studentai, char choice, std::string desc = "išvesties")
{
    switch (choice)
    {
    case '1':
        printStudentai<Container>(studentai);
        break;
    case '2':
    {
        std::string outputFileName = enterOutputFileName(desc);
        writeStudentaiToFile<Container>(studentai, outputFileName);
    }
    break;
    default:
        throw std::runtime_error("Neteisingas pasirinkimas išvesties formatui.");
    }
}

#endif // OUTPUT_H_DEFINED