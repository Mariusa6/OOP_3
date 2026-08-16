#ifndef GENERATE_H_DEFINED
#define GENERATE_H_DEFINED

#include "main.h"
#include <vector>   // vector
#include <string>   // string
#include <random>   // mt19937, random_device

// -------------------------------------------------------
// Template: generateStudentai
// Generuoja studentus su šabloniniais vardais (VardasNR1,
// PavardeNR1, ...) ir atsitiktiniais pažymiais.
// -------------------------------------------------------
template<typename Container>
Container generateStudentai(int n)
{
    Container studentai;
    for (int i = 0; i < n; i++)
    {
        studentai.push_back(studentas(
            "VardasNR" + std::to_string(i + 1),
            "PavardeNR" + std::to_string(i + 1),
            generatePazymiai(NUMBER_OF_PAZYMYS),
            generatePazymiai(1)[0]));
    }
    return studentai;
}

// -------------------------------------------------------
// Template: generateOnlyPazymiai
// Vardai ir pavardės įvedami rankiniu būdu, pažymiai generuojami.
// -------------------------------------------------------
template<typename Container>
Container generateOnlyPazymiai(int n)
{
    Container studentai;
    for (int i = 0; i < n; i++)
    {
        studentai.push_back(studentas(
            enterName(i + 1),
            enterSurname(i + 1),
            generatePazymiai(NUMBER_OF_PAZYMYS),
            generatePazymiai(1)[0]));
    }
    return studentai;
}

#endif // GENERATE_H_DEFINED