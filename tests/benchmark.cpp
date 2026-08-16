/**
 * @file benchmark.cpp
 * @brief Spartos analizė: std::vector vs nuosava Vector klasė.
 *
 * Atlieka du tyrimus:
 *  1. push_back() spartos palyginimas su 10⁴ … 10⁸ elementų
 *  2. Atminties perskirstymų skaičiavimas
 *
 * Kompiliavimas:
 *     g++ -std=c++17 -O2 benchmark.cpp -o benchmark
 *
 * Paleidimas:
 *     ./benchmark              — visi tyrimai
 *     ./benchmark 1            — tik push_back tyrimas
 *     ./benchmark 2            — tik perskirstymų tyrimas
 *
 * @warning 100 000 000 elementų testas naudoja ~400 MB kiekvienam
 *          konteineriui. Su 8 GB RAM tai priimtina, bet mažesnėse
 *          sistemose gali prireikti praleisti paskutinį dydį.
 */

#include "../OOP_2/Vector.h"

#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>

using Laikrodis = std::chrono::high_resolution_clock;
using Sekundes = std::chrono::duration<double>;

// =====================================================================
// 1 TYRIMAS — push_back sparta
// =====================================================================

/**
 * @brief Matuoja std::vector užpildymo laiką.
 * @param sz Elementų skaičius
 * @return Trukmė sekundėmis
 */
double matuokStdVector(unsigned int sz)
{
    auto pradzia = Laikrodis::now();

    std::vector<int> v1;
    for (unsigned int i = 1; i <= sz; ++i)
        v1.push_back(static_cast<int>(i));

    auto pabaiga = Laikrodis::now();
    return Sekundes(pabaiga - pradzia).count();
}

/**
 * @brief Matuoja nuosavos Vector užpildymo laiką.
 * @param sz Elementų skaičius
 * @return Trukmė sekundėmis
 */
double matuokVector(unsigned int sz)
{
    auto pradzia = Laikrodis::now();

    Vector<int> v2;
    for (unsigned int i = 1; i <= sz; ++i)
        v2.push_back(static_cast<int>(i));

    auto pabaiga = Laikrodis::now();
    return Sekundes(pabaiga - pradzia).count();
}

void tyrimas1()
{
    const unsigned int dydziai[] = {
        10000u, 100000u, 1000000u, 10000000u, 100000000u
    };

    std::cout << "\n=====================================================\n";
    std::cout << " 1 TYRIMAS - push_back() sparta\n";
    std::cout << "=====================================================\n\n";

    std::cout << std::left << std::setw(14) << "Elementu"
        << std::right << std::setw(14) << "std::vector"
        << std::setw(14) << "Vector"
        << std::setw(12) << "Santykis" << "\n";
    std::cout << std::string(54, '-') << "\n";

    for (unsigned int sz : dydziai) {
        const double t1 = matuokStdVector(sz);
        const double t2 = matuokVector(sz);
        const double santykis = t1 > 0.0 ? t2 / t1 : 0.0;

        std::cout << std::left << std::setw(14) << sz
            << std::right << std::fixed << std::setprecision(6)
            << std::setw(14) << t1
            << std::setw(14) << t2
            << std::setprecision(2) << std::setw(11) << santykis << "x"
            << "\n";
    }

    std::cout << "\nSantykis > 1.0 reiskia, kad Vector letesnis uz std::vector.\n\n";
}

// =====================================================================
// 2 TYRIMAS — atminties perskirstymai
// =====================================================================

/**
 * @brief Skaičiuoja std::vector perskirstymus.
 *
 * Perskirstymas aptinkamas stebint capacity() pokytį — jis įvyksta
 * tada, kai capacity() == size() ir pridedamas naujas elementas.
 *
 * @param sz Elementų skaičius
 * @return Perskirstymų skaičius
 */
unsigned long long skaiciuokStdVector(unsigned int sz)
{
    std::vector<int> v;
    unsigned long long perskirstymu = 0;
    std::size_t senaTalpa = v.capacity();

    for (unsigned int i = 1; i <= sz; ++i) {
        v.push_back(static_cast<int>(i));
        if (v.capacity() != senaTalpa) {
            ++perskirstymu;
            senaTalpa = v.capacity();
        }
    }
    return perskirstymu;
}

/**
 * @brief Skaičiuoja nuosavos Vector perskirstymus.
 * @param sz Elementų skaičius
 * @return Perskirstymų skaičius
 */
unsigned long long skaiciuokVector(unsigned int sz)
{
    Vector<int> v;
    unsigned long long perskirstymu = 0;
    std::size_t senaTalpa = v.capacity();

    for (unsigned int i = 1; i <= sz; ++i) {
        v.push_back(static_cast<int>(i));
        if (v.capacity() != senaTalpa) {
            ++perskirstymu;
            senaTalpa = v.capacity();
        }
    }
    return perskirstymu;
}

void tyrimas2()
{
    const unsigned int dydziai[] = {
        10000u, 100000u, 1000000u, 10000000u, 100000000u
    };

    std::cout << "\n=====================================================\n";
    std::cout << " 2 TYRIMAS - atminties perskirstymu skaicius\n";
    std::cout << "=====================================================\n\n";

    std::cout << std::left << std::setw(14) << "Elementu"
        << std::right << std::setw(14) << "std::vector"
        << std::setw(14) << "Vector" << "\n";
    std::cout << std::string(42, '-') << "\n";

    for (unsigned int sz : dydziai) {
        const auto p1 = skaiciuokStdVector(sz);
        const auto p2 = skaiciuokVector(sz);

        std::cout << std::left << std::setw(14) << sz
            << std::right << std::setw(14) << p1
            << std::setw(14) << p2 << "\n";
    }

    std::cout << "\nPerskirstymas ivyksta, kai capacity() == size() ir\n"
        << "pridedamas naujas elementas. Su dvigubinimo strategija\n"
        << "perskirstymu skaicius artimas log2(n).\n\n";
}

// =====================================================================
// Papildomas tyrimas — talpos augimo seka
// =====================================================================

void tyrimas3()
{
    std::cout << "\n=====================================================\n";
    std::cout << " 3 TYRIMAS - talpos augimo seka (pirmi 20 elementu)\n";
    std::cout << "=====================================================\n\n";

    std::cout << std::left << std::setw(10) << "size()"
        << std::setw(20) << "std::vector cap"
        << std::setw(20) << "Vector cap" << "\n";
    std::cout << std::string(50, '-') << "\n";

    std::vector<int> sv;
    Vector<int> v;

    for (int i = 1; i <= 20; ++i) {
        sv.push_back(i);
        v.push_back(i);
        std::cout << std::left << std::setw(10) << i
            << std::setw(20) << sv.capacity()
            << std::setw(20) << v.capacity() << "\n";
    }

    std::cout << "\nMSVC naudoja 1.5x augimo koeficienta, GCC/Clang - 2x.\n"
        << "Nuosava Vector naudoja 2x.\n\n";
}

// =====================================================================
// main
// =====================================================================

int main(int argc, char* argv[])
{
    std::cout << "\n";
    std::cout << "#####################################################\n";
    std::cout << "#  std::vector vs Vector - spartos analize          #\n";
    std::cout << "#####################################################\n";

    if (argc > 1) {
        const int nr = std::atoi(argv[1]);
        switch (nr) {
        case 1: tyrimas1(); break;
        case 2: tyrimas2(); break;
        case 3: tyrimas3(); break;
        default:
            std::cout << "\nNezinomas tyrimo numeris. Galimi: 1, 2, 3\n";
            return 1;
        }
    }
    else {
        tyrimas3();
        tyrimas1();
        tyrimas2();
    }

    return 0;
}