#include "calculate.h"

double mediana(const Vector<int>& namuDarbai)
{
    double mediana{ 0.0 };
    if (!namuDarbai.empty())
    {
        Vector<int> sorted = namuDarbai;
        std::sort(sorted.begin(), sorted.end());
        size_t mid{ sorted.size() / 2 };
        mediana = (sorted.size() % 2 == 0) ? (sorted[mid - 1] + sorted[mid]) / 2.0 : sorted[mid];
        return mediana;
    }
    else
         return 0.0;
}