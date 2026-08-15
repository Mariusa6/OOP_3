#ifdef _WIN32
#include <windows.h>
#endif

#ifndef MAIN_H_DEFINED
#define MAIN_H_DEFINED

#include <iostream>     // cout, cin
#include <string>       // string
#include <vector>       // vector
#include <list>         // list
#include <deque>        // deque
#include <exception>    // exception
#include <type_traits>  // is_abstract, is_base_of, is_nothrow_move_constructible
#include "zmogus.h"     // abstrakti bazinė klasė
#include "studentas.h"  // išvestinė klasė

#define NUMBER_OF_PAZYMYS 15

// -------------------------------------------------------
// splitResult — 1 strategijos rezultatas (du nauji konteineriai)
// -------------------------------------------------------
template<typename Container>
struct splitResult
{
    Container kietiakai;
    Container vargsiukai;
};

// Function declarations

// Menu functions
char askMenuChoice();
int enterNumberOfStudents();
char askSortBy();
bool askIfMoreStudents();
char askOutputChoice();
char askContainerChoice();

// Generate functions
Konteineris<int> generatePazymiai(int m);

// Print functions
void printWelcome();
void printNameAsk();

// User input functions
studentas enterStudentas(int n);
std::string enterName(int n);
std::string enterSurname(int n);
int enterNumberOfPazymys(int n);
int enterPazymys(int n);
Konteineris<int> enterPazymiai(int n, int m);
int enterEgzaminas(int n);

// File handling functions
std::string enterFileName();
std::string enterOutputFileName(std::string desc = "išvesties");

// Test functions
void testContainers(int n);
bool testStudentasKlase();      // klasių metodų testas (v1.2 / v1.5)

// Utility functions
bool isAllLetters(const std::string& input);

#endif