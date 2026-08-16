#include "menu.h"

char askMenuChoice()
{
    char choice;
    std::cout << u8"Pasirikite veiksmą:\n";
    std::cout << u8"1. Įvesti studentus ranka\n";
    std::cout << u8"2. Generuoti tik pažymius\n";
    std::cout << u8"3. Generuoti studentų vardus ir pažymius\n";
    std::cout << u8"4. Nuskaityti studentus iš failo\n";
    std::cout << u8"5. Generuoti studentų failą\n";
    std::cout << u8"6. Studentų failų generavimo testavimas\n";
    std::cout << u8"7. Duomenų apdorojimo testavimas\n";
    std::cout << u8"8. Konteinerių palyginimo testavimas\n";
    std::cout << u8"9. Klasės studentas metodų testavimas\n";
    std::cout << u8"0. Baigti darbą\n";
    std::cout << u8"Jūsų pasirinkimas: ";
    do
    {
        std::cin >> choice;
        if (choice < '0' || choice > '9')
            std::cout << u8"Netinkama įvestis, bandykite dar kartą: ";
    } while (choice < '0' || choice > '9');
    return choice;
}

int enterNumberOfStudents()
{
    int n;
    std::cout << u8"Įveskite studentų skaičių: ";
    while (!(std::cin >> n) || n <= 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Iveskite teigiamą skaičių.\n";
        std::cout << u8"Įveskite studentų skaičių: ";
    }
    return n;
}

char askSortBy()
{
    char choice;
    std::cout << u8"Rikiavimas pagal:\n"
        << u8"1. Vardą\n"
        << u8"2. Pavardę\n"
        << u8"3. Galutinį įvertinimą pagal vidurkį\n"
        << u8"4. Galutinį įvertinimą pagal medianą\n"
        << u8"Jūsų pasirinkimas: ";
    do
    {
        std::cin >> choice;
        if (choice < '1' || choice > '4')
            std::cout << u8"Netinkama įvestis, bandykite dar kartą: ";
    } while (choice < '1' || choice > '4');
    return choice;
}

char askAverageOrMedian()
{
    char choice;
    std::cout << u8"Ar norite naudoti vidurkį (v) ar medianą (m) galutiniam įvertinimui apskaičiuoti? (v/m): ";
    do
    {
        std::cin >> choice;
        choice = static_cast<char>(std::tolower(static_cast<unsigned char>(choice)));
        if (choice != 'v' && choice != 'm')
            std::cout << u8"Netinkama įvestis, bandykite 'v' arba 'm': ";
    } while (choice != 'v' && choice != 'm');
    return choice;
}

bool askIfMoreStudents()
{
    char choice;
    std::cout << u8"Ar norite įvesti dar vieną studentą? (y/n): ";
    do
    {
        std::cin >> choice;
        choice = static_cast<char>(std::tolower(static_cast<unsigned char>(choice)));
        if (choice != 'y' && choice != 'n')
            std::cout << u8"Netinkama įvestis, bandykite 'y' arba 'n': ";
    } while (choice != 'y' && choice != 'n');
    return choice == 'y';
}

char askOutputChoice()
{
    char choice;
    std::cout << u8"Pasirinkite išvesties formatą:\n"
        << u8"1. Išvesti į konsolę\n"
        << u8"2. Išvesti į failą\n"
        << u8"Jūsų pasirinkimas: ";
    do
    {
        std::cin >> choice;
        if (choice < '1' || choice > '2')
            std::cout << u8"Netinkama įvestis, bandykite dar kartą: ";
    } while (choice < '1' || choice > '2');
    return choice;
}

char askContainerChoice()
{
    char choice;
    std::cout << u8"Pasirinkite konteinerį:\n"
        << u8"1. vector\n"
        << u8"2. list\n"
        << u8"3. deque\n"
        << u8"Jūsų pasirinkimas: ";
    do
    {
        std::cin >> choice;
        if (choice < '1' || choice > '3')
            std::cout << u8"Netinkama įvestis, bandykite dar kartą: ";
    } while (choice < '1' || choice > '3');
    return choice;
}