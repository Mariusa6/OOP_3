#ifndef KONTEINERIS_H_DEFINED
#define KONTEINERIS_H_DEFINED

#include "Vector.h"
#include <vector>

/**
 * @file konteineris.h
 * @brief Konteinerio tipo perjungimas tarp std::vector ir nuosavos Vector.
 *
 * Leidžia visą programą perkompiliuoti su kitu konteineriu pakeitus
 * vieną `#define` arba kompiliavimo parametrą — taip galima palyginti
 * abiejų realizacijų spartą tomis pačiomis sąlygomis.
 *
 * @par Naudojimas
 * Pagal nutylėjimą naudojama nuosava Vector. Norint grįžti prie
 * std::vector, kompiliuoti su:
 * @code
 * g++ -DNAUDOTI_STD_VECTOR ...
 * @endcode
 * arba `CMakeLists.txt` faile įjungti `NAUDOTI_STD_VECTOR` parametrą.
 *
 * @par Pavyzdys
 * @code
 * #include "konteineris.h"
 *
 * StudentuVektorius studentai;      // Vector<studentas> arba std::vector<studentas>
 * studentai.push_back(s);
 *
 * std::cout << konteinerioPavadinimas() << "\n";   // "Vector" arba "std::vector"
 * @endcode
 */

#ifdef NAUDOTI_STD_VECTOR

 /// Naudojamo konteinerio šablonas
template <typename T>
using Konteineris = std::vector<T>;

/// @brief Grąžina naudojamo konteinerio pavadinimą (diagnostikai)
inline const char* konteinerioPavadinimas() { return "std::vector"; }

#else

 /// Naudojamo konteinerio šablonas
template <typename T>
using Konteineris = Vector<T>;

/// @brief Grąžina naudojamo konteinerio pavadinimą (diagnostikai)
inline const char* konteinerioPavadinimas() { return "Vector (nuosava)"; }

#endif

#endif // KONTEINERIS_H_DEFINED