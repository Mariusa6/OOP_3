#include "file.h"

std::string enterFileName()
{
    std::string tmp;
    std::cout << u8"Įveskite failo pavadinimą (su .txt plėtiniu): ";
    while (!(std::cin >> tmp) || tmp.size() < 5 || tmp.substr(tmp.size() - 4) != ".txt")
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Netinkama įvestis. Įveskite failo pavadinimą su .txt plėtiniu: ";
    }
    return tmp;
}

std::string enterOutputFileName(std::string desc)
{
    std::string tmp;
    std::cout << u8"Įveskite " << desc << u8" failo pavadinimą (su .txt plėtiniu): ";
    while (!(std::cin >> tmp) || tmp.size() < 5 || tmp.substr(tmp.size() - 4) != ".txt")
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Netinkama įvestis. Įveskite išvesties failo pavadinimą su .txt plėtiniu: ";
    }
    return tmp;
}