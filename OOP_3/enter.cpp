#include "enter.h"

// -------------------------------------------------------
// enterStudentas — surenka vieno studento duomenis ir sukuria
// objektą per pilną konstruktorių (jis validuoja ir apskaičiuoja).
// -------------------------------------------------------
studentas enterStudentas(int n)
{
    std::cout << '#' << n << u8" studentas:\n";

    std::string vardas = enterName(n);
    std::string pavarde = enterSurname(n);
    Vector<int> nd = enterPazymiai(n, enterNumberOfPazymys(n));
    int egzaminas = enterEgzaminas(n);

    return studentas(vardas, pavarde, nd, egzaminas);
}

std::string enterName(int n)
{
    std::string v;

    std::cout << u8"Iveskite studento #" << n << u8" vardą: ";
    while (!(std::cin >> v) || !isAllLetters(v))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Naudokite raides. Iveskite studento vardą: ";
    }

    return v;
}

std::string enterSurname(int n)
{
    std::string v;

    std::cout << u8"Iveskite studento #" << n << u8" pavardę: ";
    while (!(std::cin >> v) || !isAllLetters(v))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Naudokite raides.\n";
        std::cout << u8"Iveskite studento #" << n << u8" pavardę: ";
    }

    return v;
}

int enterNumberOfPazymys(int n)
{
    int tmp{};

    std::cout << u8"Iveskite kiek #" << n << u8" studentas turėjo namų darbų: ";
    while (!(std::cin >> tmp) || tmp < 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Iveskite neneigiamą skaičių.\n";
        std::cout << u8"Iveskite kiek #" << n << u8" studentas turėjo namų darbų: ";
    }

    return tmp;
}

int enterPazymys(int n)
{
    int tmp{};

    std::cout << n << u8" pažymys: ";
    while (!(std::cin >> tmp) || tmp < studentas::minPazymys || tmp > studentas::maxPazymys)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Iveskite skaičių nuo " << studentas::minPazymys
            << u8" iki " << studentas::maxPazymys << u8".\n";
        std::cout << n << u8" pažymys: ";
    }

    return tmp;
}

Vector<int> enterPazymiai(int n, int m) // n - studento numeris, m - pažymių skaičius
{
    Vector<int> tmp;
    std::cout << u8"Iveskite #" << n << u8" studento namų darbų pažymius.\n";
    for (int i{ 0 }; i < m; i++)
    {
        tmp.push_back(enterPazymys(i + 1));
    }
    return tmp;
}

int enterEgzaminas(int n)
{
    int tmp{};

    std::cout << u8"Iveskite kiek #" << n << u8" studentas gavo iš egzamino.\n";
    while (!(std::cin >> tmp) || tmp < studentas::minPazymys || tmp > studentas::maxPazymys)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << u8"Iveskite skaičių nuo " << studentas::minPazymys
            << u8" iki " << studentas::maxPazymys << u8".\n";
        std::cout << u8"Iveskite kiek #" << n << u8" studentas gavo iš egzamino.\n";
    }

    return tmp;
}

bool isAllLetters(const std::string& input) {
    return std::all_of(input.begin(), input.end(), ::isalpha);
}