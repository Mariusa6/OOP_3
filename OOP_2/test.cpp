#include "test.h"

// -------------------------------------------------------
// Vidinis šablonas — naudojamas tik šiame faile.
// Matuoja žingsnius su pasirinktu konteineriu:
//   1. Nuskaitymas iš failo
//   2. Galutinių balų skaičiavimas
//   3. Rikiavimas
//   4. Skirstymas į dvi grupes
// -------------------------------------------------------
template<typename Container>
static void testContainer(const std::string& label, const std::string& filename)
{
    std::cout << u8"  [" << label << u8"]\n";

    // 1. Nuskaitymas (be skaičiavimo)
    auto start = std::chrono::high_resolution_clock::now();
    Container studentai = readStudentaiFromFile<Container>(filename);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << u8"    Nuskaitymas: " << elapsed.count() << u8" s\n";

    // 2. Galutinių balų skaičiavimas (atskiras žingsnis)
    auto start2 = std::chrono::high_resolution_clock::now();
    calculateGalutinis(studentai);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end2 - start2;
    std::cout << u8"    Skaičiavimas:" << elapsed2.count() << u8" s\n";

    // 3. Rikiavimas
    // vector/deque: std::sort  |  list: .sort()  (sprendžiama if constexpr calculate.h)
    auto start3 = std::chrono::high_resolution_clock::now();
    sortStudentai(studentai, '3');  // rikiuojama pagal galutinisVid
    auto end3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed3 = end3 - start3;
    std::cout << u8"    Rikiavimas:  " << elapsed3.count() << u8" s\n";

    // 4. Skirstymas į dvi grupes (3 strategija)
    auto start4 = std::chrono::high_resolution_clock::now();
    Container vargsiukai = partitionStudentai(studentai);
    auto end4 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed4 = end4 - start4;
    std::cout << u8"    Skirstymas:  " << elapsed4.count() << u8" s\n";
}

// -------------------------------------------------------
// testContainers — iškviečiama iš meniu (pasirinkimas 8)
// Testuoja tą patį failą su visais trimis konteineriais
// -------------------------------------------------------
void testContainers(int n)
{
    const std::string filename = "data/studentai" + std::to_string(n) + ".txt";
    std::cout << u8"=== Konteinerių palyginimas: " << n << u8" studentų ===\n";
    testContainer<Konteineris<studentas>>("vector", filename);
    testContainer<std::list<studentas>>("list", filename);
    testContainer<std::deque<studentas>>("deque", filename);
    std::cout << "\n";
}