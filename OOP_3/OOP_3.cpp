#include "main.h"
#include "calculate.h"
#include "file.h"
#include "enter.h"
#include "generate.h"
#include "output.h"
#include "test.h"
#include "testStudentas.h"

// -------------------------------------------------------
// runProgram<Container> — pagrindinis programos ciklas.
// Vienas kodo kelias veikia su vector, list ir deque.
// -------------------------------------------------------
template<typename Container>
void runProgram()
{
    char choice{};
    char sortChoice{};
    char outputChoice{};
    Container studentai{}, vargsiukai{};

    while (true)
    {
        choice = askMenuChoice();
        switch (choice)
        {
        case '0':
            std::cout << u8"Programa baigta.\n";
            return;
        case '1':
            studentai = enterStudentai<Container>();
            calculateGalutinis(studentai);
            sortChoice = askSortBy();
            sortStudentai<Container>(studentai, sortChoice);
            vargsiukai = partitionStudentai(studentai);
            outputChoice = askOutputChoice();
            outputStudentai<Container>(studentai, outputChoice, "kietiakai");
            outputStudentai<Container>(vargsiukai, outputChoice, "vargsiukai");
            break;
        case '2':
            studentai = generateOnlyPazymiai<Container>(enterNumberOfStudents());
            calculateGalutinis(studentai);
            sortChoice = askSortBy();
            sortStudentai<Container>(studentai, sortChoice);
            vargsiukai = partitionStudentai(studentai);
            outputChoice = askOutputChoice();
            outputStudentai<Container>(studentai, outputChoice, "kietiakai");
            outputStudentai<Container>(vargsiukai, outputChoice, "vargsiukai");
            break;
        case '3':
            studentai = generateStudentai<Container>(enterNumberOfStudents());
            calculateGalutinis(studentai);
            sortChoice = askSortBy();
            sortStudentai<Container>(studentai, sortChoice);
            vargsiukai = partitionStudentai(studentai);
            outputChoice = askOutputChoice();
            outputStudentai<Container>(studentai, outputChoice, "kietiakai");
            outputStudentai<Container>(vargsiukai, outputChoice, "vargsiukai");
            break;
        case '4':
            studentai = readStudentaiFromFile<Container>(enterFileName());
            calculateGalutinis(studentai);
            sortChoice = askSortBy();
            sortStudentai<Container>(studentai, sortChoice);
            vargsiukai = partitionStudentai(studentai);
            outputChoice = askOutputChoice();
            outputStudentai<Container>(studentai, outputChoice, "kietiakai");
            outputStudentai<Container>(vargsiukai, outputChoice, "vargsiukai");
            break;
        case '5':
            studentai = generateStudentai<Container>(enterNumberOfStudents());
            writeStudentaiListToFile<Container>(studentai, enterOutputFileName());
            break;
        case '6':
            testGenerateStudentai<Container>(1000);
            testGenerateStudentai<Container>(10000);
            testGenerateStudentai<Container>(100000);
            testGenerateStudentai<Container>(1000000);
            testGenerateStudentai<Container>(10000000);
            break;
        case '7':
            testData<Container>(100000);
            testData<Container>(1000000);
            testData<Container>(10000000);
            break;
        case '8':
            testContainers(1000);
            testContainers(10000);
            testContainers(100000);
            testContainers(1000000);
            testContainers(10000000);
            break;
        case '9':
            // Klasės studentas metodų testas (v1.2)
            if (testStudentasKlase())
                std::cout << u8"Visi klases testai praejo sekmingai.\n\n";
            else
                std::cout << u8"DEMESIO: kai kurie klases testai nepraejo!\n\n";
            break;
        default:
            std::cout << u8"Neteisingas pasirinkimas. Bandykite dar kartą.\n";
            break;
        }
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    printWelcome();

    try
    {
        switch (askContainerChoice())
        {
        case '1':
            runProgram<Konteineris<studentas>>();
            break;
        case '2':
            runProgram<std::list<studentas>>();
            break;
        case '3':
            runProgram<std::deque<studentas>>();
            break;
        default:
            std::cout << u8"Neteisingas pasirinkimas. Programa baigta.\n";
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Klaida: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}