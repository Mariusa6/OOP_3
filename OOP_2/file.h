#ifndef FILE_H_DEFINED
#define FILE_H_DEFINED

#include "main.h"
#undef max
#include <iostream>  // cout, cin
#include <fstream>   // ifstream, ofstream
#include <sstream>   // istringstream
#include <vector>    // vector
#include <string>    // string
#include <iomanip>   // setprecision, setw
#include <limits>    // numeric_limits
#include <stdexcept> // runtime_error

// Rašymo buferio dydis — 64 KB.
constexpr size_t FILE_BUFFER_SIZE = 64 * 1024;

// -------------------------------------------------------
// Template: readStudentaiFromFile<Container>
// Nuskaito studentus iš .txt failo į bet kokį konteinerį.
//
// Optimizacijos:
//  - nenaudojamas std::istringstream (nekuriamas objektas eilutei)
//  - studentas::parseFromLine naudoja std::from_chars
//  - line buferis pernaudojamas visoms eilutėms
//  - studentas objektas kuriamas vietoje ir perkeliamas (move)
// -------------------------------------------------------
template<typename Container>
Container readStudentaiFromFile(const std::string& filename)
{
    Container studentai;
    std::ifstream file(filename);

    if (!file.is_open())
        throw std::runtime_error("Nepavyko atidaryti failo: " + filename);

    // Didesnis skaitymo buferis — mažiau OS kvietimų
    Konteineris<char> rdbuf(FILE_BUFFER_SIZE);
    file.rdbuf()->pubsetbuf(rdbuf.data(), rdbuf.size());

    std::string line;
    line.reserve(256);   // eilutės buferis pernaudojamas — jokių perskirstymų

    if (!std::getline(file, line))
        throw std::runtime_error("Failas tuščias arba sugadintas: " + filename);

    // Apskaičiuojame ND stulpelių skaičių iš antraštės
    // (antraštė nuskaitoma tik vieną kartą, todėl istringstream čia pigus)
    std::istringstream headerStream(line);
    std::string token;
    int ndCount = 0;
    while (headerStream >> token)
        if (token.find("ND") != std::string::npos) ndCount++;

    studentas s;   // objektas pernaudojamas — nekuriamas naujas kiekvienai eilutei
    int lineNumber = 1;

    while (std::getline(file, line))
    {
        lineNumber++;
        if (line.empty()) continue;

        s.parseFromLine(line, ndCount, lineNumber);
        studentai.push_back(std::move(s));
    }

    return studentai;
}

// -------------------------------------------------------
// Template: writeStudentaiToFile
// Išveda studentus su galutiniais balais.
// Formatavimą atlieka klasės operator<<.
// -------------------------------------------------------
template<typename Container>
void writeStudentaiToFile(const Container& studentai, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Nepavyko sukurti failo: " + filename);

    Konteineris<char> wrbuf(FILE_BUFFER_SIZE);
    file.rdbuf()->pubsetbuf(wrbuf.data(), wrbuf.size());

    file << u8"Vardas              Pavardė             Galutinis (Vid.)    Galutinis (Med.)\n";
    file << u8"----------------------------------------------------------------------------\n";

    for (const auto& s : studentai)
        file << s << "\n";

    file.close();
}

// -------------------------------------------------------
// Template: writeStudentaiListToFile
// Išveda studentus su visais namų darbų pažymiais.
// Šis formatas suderinamas su readStudentaiFromFile skaitymui.
//
// Optimizacijos:
//  - eilutės formuojamos į std::string buferį (studentas::appendListTo)
//    naudojant std::to_chars vietoj std::setw manipuliatorių
//  - buferis išvalomas kas ~1 MB — atmintis NEauga proporcingai
//    studentų skaičiui (skirtingai nei kaupiant viską į vieną string)
//  - vienas file.write() vietoj kelių << operacijų eilutei
// -------------------------------------------------------
template<typename Container>
void writeStudentaiListToFile(const Container& studentai, const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Nepavyko sukurti failo: " + filename);

    // Antraštė
    std::string out;
    out.reserve(1024 * 1024 + 1024);   // ~1 MB darbinis buferis

    out.append("Vardas");
    out.append(25 - 6, ' ');
    out.append("Pavarde");
    out.append(25 - 7, ' ');

    for (int i = 1; i <= NUMBER_OF_PAZYMYS; ++i)
    {
        std::string nd = "ND" + std::to_string(i);
        out.append(10 - nd.size(), ' ');
        out.append(nd);
    }
    out.append(10 - 9, ' ');
    out.append("Egzaminas");
    out.push_back('\n');

    // Eilutės — buferis išvalomas pasiekus ~1 MB ribą
    constexpr size_t FLUSH_LIMIT = 1024 * 1024;

    for (const auto& s : studentai)
    {
        s.appendListTo(out);

        if (out.size() >= FLUSH_LIMIT)
        {
            file.write(out.data(), static_cast<std::streamsize>(out.size()));
            out.clear();   // capacity išlieka — jokių naujų alokacijų
        }
    }

    // Likutis
    if (!out.empty())
        file.write(out.data(), static_cast<std::streamsize>(out.size()));

    file.close();
}

#endif // FILE_H_DEFINED