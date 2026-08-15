/**
 * @file vectorTests.cpp
 * @brief Unit testai nuosavai Vector klasei (Catch2).
 *
 * Kiekvienam testui taikomas tas pats principas: ta pati operacija
 * atliekama su `std::vector` ir su `Vector`, o rezultatai lyginami.
 * Taip tikrinama ne tik ar `Vector` veikia, bet ar veikia **taip pat**
 * kaip standartinis konteineris.
 *
 * Paleidimas:
 *     ./vectorTests                  — visi testai
 *     ./vectorTests "[talpa]"        — tik talpos testai
 *     ./vectorTests -l               — testų sąrašas
 */

#include "catch.hpp"
#include "../OOP_2/Vector.h"

#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <sstream>

 // =====================================================================
 // Pagalbinė klasė objektų gyvavimui sekti
 // =====================================================================

namespace {

    /**
     * @brief Testinė klasė, skaičiuojanti konstruktorių ir destruktorių kvietimus.
     *
     * Leidžia patikrinti, ar Vector teisingai konstruoja ir naikina
     * elementus — ypač svarbu perskirstymo metu.
     */
    struct Sekiklis {
        static int gyvu;
        static int kopiju;
        static int perkelimu;
        int reiksme;

        Sekiklis(int v = 0) : reiksme(v) { ++gyvu; }
        Sekiklis(const Sekiklis& o) : reiksme(o.reiksme) { ++gyvu; ++kopiju; }
        Sekiklis(Sekiklis&& o) noexcept : reiksme(o.reiksme) { ++gyvu; ++perkelimu; }
        Sekiklis& operator=(const Sekiklis& o) { reiksme = o.reiksme; ++kopiju; return *this; }
        Sekiklis& operator=(Sekiklis&& o) noexcept { reiksme = o.reiksme; ++perkelimu; return *this; }
        ~Sekiklis() { --gyvu; }

        bool operator==(const Sekiklis& o) const { return reiksme == o.reiksme; }

        static void atstatyk() { gyvu = 0; kopiju = 0; perkelimu = 0; }
    };

    int Sekiklis::gyvu = 0;
    int Sekiklis::kopiju = 0;
    int Sekiklis::perkelimu = 0;

    /// Palygina Vector ir std::vector turinį
    template <typename T>
    bool sutampa(const Vector<T>& v, const std::vector<T>& sv) {
        return v.size() == sv.size()
            && std::equal(v.begin(), v.end(), sv.begin());
    }

} // anoniminis namespace

// =====================================================================
// 1. KONSTRUKTORIAI
// =====================================================================

TEST_CASE("Konstruktoriai", "[konstruktoriai]")
{
    SECTION("numatytasis — tuscias vektorius") {
        Vector<int> v;
        std::vector<int> sv;
        REQUIRE(v.size() == sv.size());
        REQUIRE(v.capacity() == sv.capacity());
        REQUIRE(v.empty() == sv.empty());
        REQUIRE(v.data() == nullptr);
    }

    SECTION("n elementu su reiksme") {
        Vector<int> v(5, 42);
        std::vector<int> sv(5, 42);
        REQUIRE(sutampa(v, sv));
        REQUIRE(v.size() == 5);
        REQUIRE(v[0] == 42);
        REQUIRE(v[4] == 42);
    }

    SECTION("n numatytai sukonstruotu elementu") {
        Vector<int> v(5);
        std::vector<int> sv(5);
        REQUIRE(sutampa(v, sv));
        REQUIRE(v[0] == 0);   // int() = 0
    }

    SECTION("is iteratoriu intervalo") {
        std::vector<int> saltinis = { 1, 2, 3, 4, 5 };
        Vector<int> v(saltinis.begin(), saltinis.end());
        REQUIRE(sutampa(v, saltinis));
    }

    SECTION("is initializer_list") {
        Vector<int> v = { 1, 2, 3 };
        std::vector<int> sv = { 1, 2, 3 };
        REQUIRE(sutampa(v, sv));
    }

    SECTION("Vector(5, 10) NEinterpretuojamas kaip intervalas") {
        // Be enable_if sios eilutes butu klaidingai parinktas
        // intervalo konstruktorius
        Vector<int> v(5, 10);
        REQUIRE(v.size() == 5);
        REQUIRE(v[0] == 10);
    }

    SECTION("kopijavimo konstruktorius — gili kopija") {
        Vector<int> originalas = { 1, 2, 3 };
        Vector<int> kopija(originalas);

        REQUIRE(kopija == originalas);
        REQUIRE(kopija.data() != originalas.data());   // skirtinga atmintis

        kopija[0] = 99;
        REQUIRE(originalas[0] == 1);                   // originalas nepakito
    }

    SECTION("perkelimo konstruktorius") {
        Vector<int> saltinis = { 1, 2, 3 };
        const int* senasAdresas = saltinis.data();

        Vector<int> tikslas(std::move(saltinis));

        REQUIRE(tikslas.size() == 3);
        REQUIRE(tikslas.data() == senasAdresas);       // ta pati atmintis
        REQUIRE(saltinis.size() == 0);                 // saltinis tuscias
        REQUIRE(saltinis.data() == nullptr);
    }

    SECTION("konstruktoriai teisingai naikina objektus") {
        Sekiklis::atstatyk();
        {
            Vector<Sekiklis> v(10);
            REQUIRE(Sekiklis::gyvu == 10);
        }
        REQUIRE(Sekiklis::gyvu == 0);
    }
}

// =====================================================================
// 2. PRISKYRIMAS
// =====================================================================

TEST_CASE("Priskyrimas", "[priskyrimas]")
{
    SECTION("kopijavimo priskyrimas") {
        Vector<int> a = { 1, 2, 3 };
        Vector<int> b;
        b = a;
        REQUIRE(b == a);
        REQUIRE(b.data() != a.data());
    }

    SECTION("priskyrimas sau nesugadina") {
        Vector<int> a = { 1, 2, 3 };
        a = a;
        REQUIRE(a.size() == 3);
        REQUIRE(a[0] == 1);
    }

    SECTION("perkelimo priskyrimas") {
        Vector<int> a = { 1, 2, 3 };
        Vector<int> b = { 9 };
        b = std::move(a);
        REQUIRE(b.size() == 3);
        REQUIRE(a.size() == 0);
    }

    SECTION("priskyrimas is initializer_list") {
        Vector<int> v;
        v = { 4, 5, 6 };
        REQUIRE(v.size() == 3);
        REQUIRE(v[1] == 5);
    }

    SECTION("assign su n kopiju") {
        Vector<int> v = { 1, 2, 3 };
        std::vector<int> sv = { 1, 2, 3 };
        v.assign(5, 7);
        sv.assign(5, 7);
        REQUIRE(sutampa(v, sv));
    }

    SECTION("assign su intervalu") {
        std::vector<int> saltinis = { 8, 9 };
        Vector<int> v = { 1, 2, 3 };
        v.assign(saltinis.begin(), saltinis.end());
        REQUIRE(sutampa(v, saltinis));
    }
}

// =====================================================================
// 3. ELEMENTŲ PRIEIGA
// =====================================================================

TEST_CASE("Elementu prieiga", "[prieiga]")
{
    Vector<int> v = { 10, 20, 30, 40 };
    std::vector<int> sv = { 10, 20, 30, 40 };

    SECTION("operator[]") {
        REQUIRE(v[0] == sv[0]);
        REQUIRE(v[3] == sv[3]);
        v[1] = 99;
        REQUIRE(v[1] == 99);
    }

    SECTION("at() su ribu tikrinimu") {
        REQUIRE(v.at(2) == sv.at(2));
        REQUIRE_THROWS_AS(v.at(4), std::out_of_range);
        REQUIRE_THROWS_AS(v.at(100), std::out_of_range);

        // std::vector meta ta pacia isimti
        REQUIRE_THROWS_AS(sv.at(4), std::out_of_range);
    }

    SECTION("front ir back") {
        REQUIRE(v.front() == sv.front());
        REQUIRE(v.back() == sv.back());
        v.front() = 1;
        v.back() = 4;
        REQUIRE(v[0] == 1);
        REQUIRE(v[3] == 4);
    }

    SECTION("data() grazina rodykle i masyva") {
        int* p = v.data();
        REQUIRE(p[0] == 10);
        REQUIRE(p[3] == 40);
    }

    SECTION("const prieiga") {
        const Vector<int>& cv = v;
        REQUIRE(cv[0] == 10);
        REQUIRE(cv.at(1) == 20);
        REQUIRE(cv.front() == 10);
        REQUIRE(cv.back() == 40);
        REQUIRE(cv.data() != nullptr);
    }
}

// =====================================================================
// 4. ITERATORIAI
// =====================================================================

TEST_CASE("Iteratoriai", "[iteratoriai]")
{
    Vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<int> sv = { 1, 2, 3, 4, 5 };

    SECTION("begin ir end") {
        REQUIRE(*v.begin() == 1);
        REQUIRE(*(v.end() - 1) == 5);
        REQUIRE(v.end() - v.begin() == 5);
    }

    SECTION("range-based for") {
        int suma = 0;
        for (int x : v) suma += x;
        REQUIRE(suma == 15);
    }

    SECTION("atvirkstiniai iteratoriai") {
        std::vector<int> atvirksciai;
        for (auto it = v.rbegin(); it != v.rend(); ++it)
            atvirksciai.push_back(*it);
        REQUIRE(atvirksciai == std::vector<int>{ 5, 4, 3, 2, 1 });
    }

    SECTION("const iteratoriai") {
        const Vector<int>& cv = v;
        REQUIRE(*cv.cbegin() == 1);
        REQUIRE(*(cv.cend() - 1) == 5);
        REQUIRE(*cv.crbegin() == 5);
    }

    SECTION("veikia su STL algoritmais") {
        REQUIRE(std::accumulate(v.begin(), v.end(), 0) == 15);
        REQUIRE(*std::max_element(v.begin(), v.end()) == 5);
        REQUIRE(std::count(v.begin(), v.end(), 3) == 1);

        std::sort(v.begin(), v.end(), std::greater<int>());
        REQUIRE(v[0] == 5);

        REQUIRE(std::find(v.begin(), v.end(), 3) != v.end());
    }
}

// =====================================================================
// 5. TALPA — svarbiausias skyrius
// =====================================================================

TEST_CASE("Talpa", "[talpa]")
{
    SECTION("empty ir size") {
        Vector<int> v;
        REQUIRE(v.empty());
        REQUIRE(v.size() == 0);

        v.push_back(1);
        REQUIRE_FALSE(v.empty());
        REQUIRE(v.size() == 1);
    }

    SECTION("capacity dvigubinama augant") {
        Vector<int> v;
        REQUIRE(v.capacity() == 0);

        v.push_back(1);
        REQUIRE(v.capacity() == 1);

        v.push_back(2);
        REQUIRE(v.capacity() == 2);

        v.push_back(3);
        REQUIRE(v.capacity() == 4);

        v.push_back(4);
        REQUIRE(v.capacity() == 4);

        v.push_back(5);
        REQUIRE(v.capacity() == 8);
    }

    SECTION("reserve paskiria atminti, bet nekuria elementu") {
        Vector<int> v;
        v.reserve(100);
        REQUIRE(v.capacity() >= 100);
        REQUIRE(v.size() == 0);
        REQUIRE(v.empty());
    }

    SECTION("reserve nemazina talpos") {
        Vector<int> v;
        v.reserve(100);
        v.reserve(10);
        REQUIRE(v.capacity() >= 100);
    }

    SECTION("reserve panaikina perskirstymus") {
        Vector<int> v;
        v.reserve(1000);
        const int* pries = v.data();

        for (int i = 0; i < 1000; ++i)
            v.push_back(i);

        REQUIRE(v.data() == pries);   // atmintis nepersikele
    }

    SECTION("shrink_to_fit atlaisvina nepanaudota atminti") {
        Vector<int> v;
        v.reserve(100);
        v.push_back(1);
        v.push_back(2);
        REQUIRE(v.capacity() >= 100);

        v.shrink_to_fit();
        REQUIRE(v.capacity() == 2);
        REQUIRE(v.size() == 2);
        REQUIRE(v[0] == 1);
    }

    SECTION("max_size grazina teigiama reiksme") {
        Vector<int> v;
        REQUIRE(v.max_size() > 0);
    }

    SECTION("perskirstymu skaicius atitinka log2(n)") {
        Vector<int> v;
        int perskirstymu = 0;
        size_t senaTalpa = v.capacity();

        for (int i = 0; i < 1000; ++i) {
            v.push_back(i);
            if (v.capacity() != senaTalpa) {
                ++perskirstymu;
                senaTalpa = v.capacity();
            }
        }
        // 1000 elementu → talpa 1,2,4,...,1024 → 11 perskirstymu
        REQUIRE(perskirstymu == 11);
    }
}

// =====================================================================
// 6. MODIFIKAVIMAS
// =====================================================================

TEST_CASE("push_back ir pop_back", "[modifikavimas]")
{
    SECTION("push_back prideda i gala") {
        Vector<int> v;
        std::vector<int> sv;
        for (int i = 1; i <= 10; ++i) { v.push_back(i); sv.push_back(i); }
        REQUIRE(sutampa(v, sv));
    }

    SECTION("push_back su perkelimu") {
        Vector<std::string> v;
        std::string s = "labas";
        v.push_back(std::move(s));
        REQUIRE(v[0] == "labas");
        REQUIRE(s.empty());   // perkelta
    }

    SECTION("pop_back pasalina paskutini") {
        Vector<int> v = { 1, 2, 3 };
        v.pop_back();
        REQUIRE(v.size() == 2);
        REQUIRE(v.back() == 2);
    }

    SECTION("pop_back sunaikina objekta") {
        Sekiklis::atstatyk();
        Vector<Sekiklis> v;
        v.push_back(Sekiklis(1));
        v.push_back(Sekiklis(2));
        const int pries = Sekiklis::gyvu;
        v.pop_back();
        REQUIRE(Sekiklis::gyvu == pries - 1);
    }
}

TEST_CASE("emplace_back", "[modifikavimas]")
{
    SECTION("sukuria objekta vietoje") {
        Vector<std::string> v;
        v.emplace_back(5, 'a');       // std::string(5, 'a')
        REQUIRE(v[0] == "aaaaa");
    }

    SECTION("grazina nuoroda i sukurta elementa") {
        Vector<int> v;
        int& r = v.emplace_back(42);
        REQUIRE(r == 42);
        r = 99;
        REQUIRE(v[0] == 99);
    }

    SECTION("emplace_back nekopijuoja") {
        Sekiklis::atstatyk();
        Vector<Sekiklis> v;
        v.reserve(10);
        v.emplace_back(5);
        REQUIRE(Sekiklis::kopiju == 0);   // jokiu kopiju
    }
}

TEST_CASE("insert", "[modifikavimas]")
{
    SECTION("insert i pradzia") {
        Vector<int> v = { 2, 3 };
        std::vector<int> sv = { 2, 3 };
        v.insert(v.begin(), 1);
        sv.insert(sv.begin(), 1);
        REQUIRE(sutampa(v, sv));
    }

    SECTION("insert i vidury") {
        Vector<int> v = { 1, 3 };
        std::vector<int> sv = { 1, 3 };
        v.insert(v.begin() + 1, 2);
        sv.insert(sv.begin() + 1, 2);
        REQUIRE(sutampa(v, sv));
    }

    SECTION("insert i gala") {
        Vector<int> v = { 1, 2 };
        v.insert(v.end(), 3);
        REQUIRE(v.size() == 3);
        REQUIRE(v.back() == 3);
    }

    SECTION("insert n kopiju") {
        Vector<int> v = { 1, 5 };
        std::vector<int> sv = { 1, 5 };
        v.insert(v.begin() + 1, 3, 9);
        sv.insert(sv.begin() + 1, 3, 9);
        REQUIRE(sutampa(v, sv));
    }

    SECTION("insert intervala") {
        std::vector<int> saltinis = { 2, 3, 4 };
        Vector<int> v = { 1, 5 };
        std::vector<int> sv = { 1, 5 };
        v.insert(v.begin() + 1, saltinis.begin(), saltinis.end());
        sv.insert(sv.begin() + 1, saltinis.begin(), saltinis.end());
        REQUIRE(sutampa(v, sv));
    }

    SECTION("insert initializer_list") {
        Vector<int> v = { 1, 4 };
        v.insert(v.begin() + 1, { 2, 3 });
        REQUIRE(v.size() == 4);
        REQUIRE(v[1] == 2);
        REQUIRE(v[2] == 3);
    }

    SECTION("insert grazina iteratoriu i iterpta elementa") {
        Vector<int> v = { 1, 3 };
        auto it = v.insert(v.begin() + 1, 2);
        REQUIRE(*it == 2);
        REQUIRE(it == v.begin() + 1);
    }
}

TEST_CASE("erase", "[modifikavimas]")
{
    SECTION("erase viena elementa") {
        Vector<int> v = { 1, 2, 3, 4 };
        std::vector<int> sv = { 1, 2, 3, 4 };
        v.erase(v.begin() + 1);
        sv.erase(sv.begin() + 1);
        REQUIRE(sutampa(v, sv));
    }

    SECTION("erase intervala") {
        Vector<int> v = { 1, 2, 3, 4, 5 };
        std::vector<int> sv = { 1, 2, 3, 4, 5 };
        v.erase(v.begin() + 1, v.begin() + 4);
        sv.erase(sv.begin() + 1, sv.begin() + 4);
        REQUIRE(sutampa(v, sv));
    }

    SECTION("erase grazina iteratoriu i kita elementa") {
        Vector<int> v = { 1, 2, 3 };
        auto it = v.erase(v.begin());
        REQUIRE(*it == 2);
    }

    SECTION("erase sunaikina objektus") {
        Sekiklis::atstatyk();
        Vector<Sekiklis> v;
        for (int i = 0; i < 5; ++i) v.emplace_back(i);
        const int pries = Sekiklis::gyvu;
        v.erase(v.begin(), v.begin() + 2);
        REQUIRE(Sekiklis::gyvu == pries - 2);
    }
}

TEST_CASE("clear ir resize", "[modifikavimas]")
{
    SECTION("clear pasalina elementus, bet ne talpa") {
        Vector<int> v = { 1, 2, 3 };
        const size_t talpa = v.capacity();
        v.clear();
        REQUIRE(v.size() == 0);
        REQUIRE(v.empty());
        REQUIRE(v.capacity() == talpa);   // talpa nepakito
    }

    SECTION("clear sunaikina objektus") {
        Sekiklis::atstatyk();
        Vector<Sekiklis> v;
        for (int i = 0; i < 5; ++i) v.emplace_back(i);
        v.clear();
        REQUIRE(Sekiklis::gyvu == 0);
    }

    SECTION("resize didinant") {
        Vector<int> v = { 1, 2 };
        std::vector<int> sv = { 1, 2 };
        v.resize(5);
        sv.resize(5);
        REQUIRE(sutampa(v, sv));
        REQUIRE(v[4] == 0);
    }

    SECTION("resize didinant su reiksme") {
        Vector<int> v = { 1, 2 };
        std::vector<int> sv = { 1, 2 };
        v.resize(5, 9);
        sv.resize(5, 9);
        REQUIRE(sutampa(v, sv));
        REQUIRE(v[4] == 9);
    }

    SECTION("resize mazinant") {
        Vector<int> v = { 1, 2, 3, 4, 5 };
        v.resize(2);
        REQUIRE(v.size() == 2);
        REQUIRE(v.back() == 2);
    }
}

TEST_CASE("swap", "[modifikavimas]")
{
    SECTION("nario swap") {
        Vector<int> a = { 1, 2 };
        Vector<int> b = { 3, 4, 5 };
        const int* aData = a.data();
        const int* bData = b.data();

        a.swap(b);

        REQUIRE(a.size() == 3);
        REQUIRE(b.size() == 2);
        REQUIRE(a.data() == bData);   // sukeistos rodykles, ne elementai
        REQUIRE(b.data() == aData);
    }

    SECTION("ne-nare swap funkcija") {
        Vector<int> a = { 1 };
        Vector<int> b = { 2 };
        swap(a, b);
        REQUIRE(a[0] == 2);
        REQUIRE(b[0] == 1);
    }
}

// =====================================================================
// 7. NE-NARIAI OPERATORIAI
// =====================================================================

TEST_CASE("Lyginimo operatoriai", "[operatoriai]")
{
    Vector<int> a = { 1, 2, 3 };
    Vector<int> b = { 1, 2, 3 };
    Vector<int> c = { 1, 2, 4 };
    Vector<int> trumpas = { 1, 2 };

    SECTION("== ir !=") {
        REQUIRE(a == b);
        REQUIRE(a != c);
        REQUIRE(a != trumpas);
    }

    SECTION("leksikografinis palyginimas") {
        REQUIRE(a < c);
        REQUIRE(c > a);
        REQUIRE(trumpas < a);
        REQUIRE(a <= b);
        REQUIRE(a >= b);
    }

    SECTION("rezultatai sutampa su std::vector") {
        std::vector<int> sa = { 1, 2, 3 };
        std::vector<int> sc = { 1, 2, 4 };
        REQUIRE((a < c) == (sa < sc));
        REQUIRE((a == b) == (sa == std::vector<int>{ 1, 2, 3 }));
    }
}

TEST_CASE("erase ir erase_if (ne-nares)", "[operatoriai]")
{
    SECTION("erase pasalina visas reiksmes kopijas") {
        Vector<int> v = { 1, 2, 3, 2, 5, 2 };
        const auto n = erase(v, 2);
        REQUIRE(n == 3);
        REQUIRE(v.size() == 3);
        REQUIRE(v == Vector<int>{ 1, 3, 5 });
    }

    SECTION("erase_if pagal predikata") {
        Vector<int> v = { 1, 2, 3, 4, 5, 6 };
        const auto n = erase_if(v, [](int x) { return x % 2 == 0; });
        REQUIRE(n == 3);
        REQUIRE(v == Vector<int>{ 1, 3, 5 });
    }
}

// =====================================================================
// 8. SUDĖTINGI TIPAI
// =====================================================================

TEST_CASE("Veikimas su sudetingais tipais", "[tipai]")
{
    SECTION("Vector<std::string>") {
        Vector<std::string> v;
        v.push_back("pirmas");
        v.push_back("antras");
        v.emplace_back("trecias");

        REQUIRE(v.size() == 3);
        REQUIRE(v[0] == "pirmas");
        REQUIRE(v.back() == "trecias");
    }

    SECTION("Vector<Vector<int>> — vektorius vektoriuje") {
        Vector<Vector<int>> matrica;
        matrica.push_back(Vector<int>{ 1, 2 });
        matrica.push_back(Vector<int>{ 3, 4 });

        REQUIRE(matrica.size() == 2);
        REQUIRE(matrica[0][1] == 2);
        REQUIRE(matrica[1][0] == 3);
    }

    SECTION("perskirstymas nesugadina objektu") {
        Sekiklis::atstatyk();
        {
            Vector<Sekiklis> v;
            for (int i = 0; i < 100; ++i)
                v.emplace_back(i);

            REQUIRE(Sekiklis::gyvu == 100);
            REQUIRE(v[0].reiksme == 0);
            REQUIRE(v[99].reiksme == 99);
        }
        REQUIRE(Sekiklis::gyvu == 0);   // nera nutekejimo
    }

    SECTION("perskirstymas naudoja move, ne copy") {
        Sekiklis::atstatyk();
        Vector<Sekiklis> v;
        v.reserve(2);
        v.emplace_back(1);
        v.emplace_back(2);

        const int kopijuPries = Sekiklis::kopiju;
        v.emplace_back(3);   // sukelia perskirstyma

        // Sekiklis turi noexcept move, todel turi buti naudojamas
        REQUIRE(Sekiklis::kopiju == kopijuPries);
        REQUIRE(Sekiklis::perkelimu > 0);
    }
}

// =====================================================================
// 9. KRAŠTINIAI ATVEJAI
// =====================================================================

TEST_CASE("Krastiniai atvejai", "[krastiniai]")
{
    SECTION("operacijos su tusciu vektoriumi") {
        Vector<int> v;
        REQUIRE(v.begin() == v.end());
        REQUIRE(v.size() == 0);
        REQUIRE_NOTHROW(v.clear());
        REQUIRE_NOTHROW(v.shrink_to_fit());
        REQUIRE_THROWS_AS(v.at(0), std::out_of_range);
    }

    SECTION("insert i tuscia vektoriu") {
        Vector<int> v;
        v.insert(v.begin(), 1);
        REQUIRE(v.size() == 1);
        REQUIRE(v[0] == 1);
    }

    SECTION("erase visus elementus") {
        Vector<int> v = { 1, 2, 3 };
        v.erase(v.begin(), v.end());
        REQUIRE(v.empty());
    }

    SECTION("insert nulis elementu") {
        Vector<int> v = { 1, 2 };
        v.insert(v.begin(), 0, 9);
        REQUIRE(v.size() == 2);
    }

    SECTION("resize i ta pati dydi") {
        Vector<int> v = { 1, 2, 3 };
        v.resize(3);
        REQUIRE(v.size() == 3);
        REQUIRE(v[2] == 3);
    }

    SECTION("didelis reserve") {
        Vector<int> v;
        REQUIRE_NOTHROW(v.reserve(100000));
        REQUIRE(v.capacity() >= 100000);
    }
}