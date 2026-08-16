#ifndef PRINT_H_DEFINED
#define PRINT_H_DEFINED

#include "main.h"
#include <iostream>  // cout, cin
#include <iomanip>   // setprecision, setw
#include <vector>    // vector
#include <stdexcept> // runtime_error

// -------------------------------------------------------
// printStudentai — išveda studentus į konsolę su antrašte.
// -------------------------------------------------------
template<typename Container>
void printStudentai(const Container& studentai)
{
    if (studentai.empty())
        throw std::runtime_error("Studentų sąrašas tuščias — nėra ko spausdinti.");

    std::cout << std::left << std::setw(20) << u8"Vardas"
        << std::left << std::setw(20) << u8"Pavardė"
        << std::left << std::setw(20) << u8"Galutinis (Vid.)"
        << std::left << std::setw(20) << u8"Galutinis (Med.)" << "\n";
    std::cout << u8"-----------------------------------------------------------------------\n";

    for (const auto& s : studentai)
        std::cout << s << "\n";
}

#endif // PRINT_H_DEFINED