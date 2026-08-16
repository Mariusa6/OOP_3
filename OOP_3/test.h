#ifndef TEST_H_DEFINED
#define TEST_H_DEFINED

#include "main.h"
#include "file.h"
#include "generate.h"
#include "calculate.h"
#include <chrono>

// -------------------------------------------------------
// testGenerateStudentai — iškviečiama iš meniu (pasirinkimas 6)
// Testuoja studentų failo generavimą su n studentų.
// -------------------------------------------------------
template<typename Container>
void testGenerateStudentai(int n)
{
    std::cout << u8"=== Studentų failo generavimo testavimas: " << n << u8" studentų ===\n";

    auto start = std::chrono::high_resolution_clock::now();
    Container studentai = generateStudentai<Container>(n);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << u8"  Generavimas: " << elapsed.count() << u8" s\n";

    start = std::chrono::high_resolution_clock::now();
    writeStudentaiListToFile<Container>(studentai, "teststudentai" + std::to_string(n) + ".txt");
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << u8"  Išvedimas į failą: " << elapsed.count() << u8" s\n";
}

// -------------------------------------------------------
// testData — iškviečiama iš meniu (pasirinkimas 7)
// Testuoja studentų duomenų apdorojimą su n studentų.
// -------------------------------------------------------
template<typename Container>
void testData(int n)
{
    auto startTotal = std::chrono::high_resolution_clock::now();
    std::cout << u8"Pradedamas testavimas su " << n << u8" studentų duomenimis.\n";
    std::cout << u8"------------------------------------------------------------\n";

    auto start = std::chrono::high_resolution_clock::now();
    Container studentai = readStudentaiFromFile<Container>(
        "data/studentai" + std::to_string(n) + ".txt");
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << n << u8" failo skaitymas užtruko: " << elapsed.count() << u8" sekundžių.\n";

	auto start2 = std::chrono::high_resolution_clock::now();
    calculateGalutinis(studentai);
	auto end2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed2 = end2 - start2;
	std::cout << n << u8" galutinio balo skaičiavimas užtruko: "
		<< elapsed2.count() << u8" sekundžių.\n";

	auto start3 = std::chrono::high_resolution_clock::now();
    sortStudentai(studentai, '4');
	auto end3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed3 = end3 - start3;
	std::cout << n << u8" rikiavimas pagal medianą užtruko: "
		<< elapsed3.count() << u8" sekundžių.\n";

    auto start4 = std::chrono::high_resolution_clock::now();
    splitResult<Container> result = splitStudentai<Container>(studentai);
    auto end4 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed4 = end4 - start4;
    std::cout << n << u8" skirstymas į kietiakai ir vargsiukai užtruko: "
        << elapsed4.count() << u8" sekundžių.\n";

    auto start5 = std::chrono::high_resolution_clock::now();
    writeStudentaiListToFile<Container>(result.kietiakai, "testkietiakai" + std::to_string(n) + ".txt");
    auto end5 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed5 = end5 - start5;
    std::cout << n << u8" Kietiakai failo generavimas užtruko: "
        << elapsed5.count() << u8" sekundžių.\n";

    auto start6 = std::chrono::high_resolution_clock::now();
    writeStudentaiListToFile<Container>(result.vargsiukai, "testvargsiukai" + std::to_string(n) + ".txt");
    auto end6 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed6 = end6 - start6;

    auto endTotal = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTotal = endTotal - startTotal;
    std::cout << n << u8" Vargsiukai failo generavimas užtruko: "
        << elapsed6.count() << u8" sekundžių.\n";
    std::cout << n << u8" Visas testavimas užtruko: "
        << elapsedTotal.count() << u8" sekundžių.\n\n";
}

#endif