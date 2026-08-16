#ifndef ENTER_H_DEFINED
#define ENTER_H_DEFINED

#include "main.h"
#undef max
#include <iostream> // cout, cin
#include <limits>   // numeric_limits
#include <ios>      // streamsize
#include <vector>   // vector
#include <string>   // string
#include <algorithm> // all_of
#include <cctype>   // isalpha

// -------------------------------------------------------
// Template: enterStudentai<Container>
// Įveda studentus ranka į bet kokį konteinerį
// (vector, list, deque) naudojant push_back.
// -------------------------------------------------------

template<typename Container>
Container enterStudentai()
{
    Container tmp;
    int n{ 1 };
    do
    {
        tmp.push_back(enterStudentas(n));
        n++;
    } while (askIfMoreStudents());

    return tmp;
}

#endif // ENTER_H_DEFINED