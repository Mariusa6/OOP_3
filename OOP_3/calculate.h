#ifndef CALCULATE_H_DEFINED
#define CALCULATE_H_DEFINED

#include "main.h"
#include <vector>       // vector
#include <algorithm>    // sort, partition_copy, stable_partition
#include <iterator>     // back_inserter, make_move_iterator
#include <type_traits>  // is_same_v
#include <stdexcept>    // runtime_error

// -------------------------------------------------------
// Template: calculateGalutinis
// -------------------------------------------------------
template<typename Container>
void calculateGalutinis(Container& studentai)
{
    if (studentai.empty())
        throw std::runtime_error("Studentų sąrašas tuščias — nėra ką skaičiuoti.");

    for (auto& s : studentai)
        s.calculateGalutinis();
}

// -------------------------------------------------------
// Template: sortStudentai
// -------------------------------------------------------
template<typename Container>
void sortStudentai(Container& studentai, char sortBy)
{
    auto cmp = [sortBy](const studentas& a, const studentas& b) -> bool
        {
            switch (sortBy)
            {
            case '1': return comparePagalVarda(a, b);
            case '2': return comparePagalPavarde(a, b);
            case '3': return comparePagalVidurki(a, b);
            case '4': return comparePagalMediana(a, b);
            default:  return false;
            }
        };

    if constexpr (std::is_same_v<Container, std::list<studentas>>)
        studentai.sort(cmp);                                 // list narys
    else
        std::sort(studentai.begin(), studentai.end(), cmp);  // vector / deque
}

// -------------------------------------------------------
// Template: splitStudentai  (1 strategija)
// Skaido į DU naujus konteinerius, originalas nekeičiamas.
// Neefektyvu atminties atžvilgiu — duomenys dublikuojami.
// -------------------------------------------------------
template<typename Container>
splitResult<Container> splitStudentai(const Container& studentai)
{
    splitResult<Container> result;

    std::partition_copy(studentai.begin(), studentai.end(),
        std::back_inserter(result.kietiakai),
        std::back_inserter(result.vargsiukai),
        [](const studentas& s) { return s.galutinisVid() >= 5.0; });

    return result;
}

// -------------------------------------------------------
// Template: partitionStudentai  (3 strategija — optimaliausia)
// Perskirsto originalų konteinerį vietoje, vargsiukus perkelia
// į naują konteinerį ir blokiniu erase pašalina iš originalo.
// Po iškvietimo studentai konteineryje lieka tik kietiakai.
// -------------------------------------------------------
template<typename Container>
Container partitionStudentai(Container& studentai)
{
    auto it = std::stable_partition(studentai.begin(), studentai.end(),
        [](const studentas& s) { return s.galutinisVid() >= 5.0; });

    Container vargsiukai(std::make_move_iterator(it),
        std::make_move_iterator(studentai.end()));
    studentai.erase(it, studentai.end());

    return vargsiukai;
}

#endif // CALCULATE_H_DEFINED