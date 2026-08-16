/**
 * @file unitTests.cpp
 * @brief Unit testai klasėms zmogus ir studentas (Catch2 framework).
 *
 * Testai suskirstyti į sekcijas pagal testuojamą klasės aspektą.
 * Pagrindinis dėmesys — penkių metodų taisyklė (Rule of Five),
 * nes būtent ji lengviausiai sugenda tyliai: pamiršus bazinės klasės
 * kvietimą arba noexcept, programa vis tiek kompiliuosis ir veiks,
 * tik lėčiau arba su prarastais duomenimis.
 *
 * Paleidimas:
 *     ./unitTests              — visi testai
 *     ./unitTests -s           — su detaliu išvedimu
 *     ./unitTests "[rule5]"    — tik Rule of Five testai
 *     ./unitTests -l           — testų sąrašas
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../OOP_3/zmogus.h"
#include "../OOP_3/studentas.h"

#include <sstream>
#include <vector>
#include <memory>
#include <type_traits>

// =====================================================================
// Pagalbinės funkcijos
// =====================================================================

namespace {

    /**
     * @brief Sukuria testinį studentą su žinomomis reikšmėmis.
     *
     * ND: 5,5,5,5,5 → vidurkis 5.0, mediana 5.0
     * Egzaminas 10  → galutinis = 0.4*5.0 + 0.6*10 = 8.0
     */
    studentas kurkTestini()
    {
        return studentas("Jonas", "Jonaitis", { 5, 5, 5, 5, 5 }, 10);
    }

} // anoniminis namespace

// =====================================================================
// 1. ABSTRAKČIOJI KLASĖ
// =====================================================================

TEST_CASE("zmogus yra abstrakti klase", "[abstrakti]")
{
    SECTION("is_abstract patikra") {
        REQUIRE(std::is_abstract<zmogus>::value);
        REQUIRE_FALSE(std::is_abstract<studentas>::value);
    }

    SECTION("zmogus objekto sukurti neimanoma") {
        // Sis kodas NESIKOMPILIUOTU:
        //     zmogus z;
        //     zmogus* p = new zmogus();
        // Todel patikra atliekama statiskai:
        REQUIRE_FALSE(std::is_default_constructible<zmogus>::value);
        REQUIRE(std::is_default_constructible<studentas>::value);
    }

    SECTION("paveldejimo rysys") {
        REQUIRE(std::is_base_of<zmogus, studentas>::value);
        REQUIRE(std::is_convertible<studentas*, zmogus*>::value);
    }

    SECTION("virtualus destruktorius") {
        REQUIRE(std::has_virtual_destructor<zmogus>::value);
    }
}

// =====================================================================
// 2. KONSTRUKTORIAI
// =====================================================================

TEST_CASE("Konstruktoriai", "[konstruktoriai]")
{
    SECTION("numatytasis konstruktorius inicijuoja visus laukus") {
        studentas s;
        REQUIRE(s.vardas().empty());
        REQUIRE(s.pavarde().empty());
        REQUIRE(s.nd().empty());
        REQUIRE(s.egzaminas() == 0);
        REQUIRE(s.galutinisVid() == Approx(0.0));
        REQUIRE(s.galutinisMed() == Approx(0.0));
        REQUIRE_FALSE(static_cast<bool>(s));
    }

    SECTION("pilnas konstruktorius issaugo duomenis") {
        studentas s("Jonas", "Jonaitis", { 5, 6, 7 }, 8);
        REQUIRE(s.vardas() == "Jonas");
        REQUIRE(s.pavarde() == "Jonaitis");
        REQUIRE(s.nd().size() == 3);
        REQUIRE(s.egzaminas() == 8);
        REQUIRE(static_cast<bool>(s));
    }

    SECTION("konstruktorius is srauto") {
        std::istringstream iss("Petras Petraitis 8 9 10 7");
        studentas s(iss);
        REQUIRE(s.vardas() == "Petras");
        REQUIRE(s.pavarde() == "Petraitis");
        REQUIRE(s.nd().size() == 3);
        REQUIRE(s.egzaminas() == 7);
    }
}

TEST_CASE("Konstruktoriaus validacija", "[konstruktoriai][validacija]")
{
    SECTION("bazine klase meta invalid_argument tusciam vardui") {
        REQUIRE_THROWS_AS(studentas("", "Jonaitis", { 5 }, 10),
                          std::invalid_argument);
    }

    SECTION("bazine klase meta invalid_argument tusciai pavardei") {
        REQUIRE_THROWS_AS(studentas("Jonas", "", { 5 }, 10),
                          std::invalid_argument);
    }

    SECTION("isvestine klase meta runtime_error blogam ND pazymiui") {
        REQUIRE_THROWS_AS(studentas("Jonas", "Jonaitis", { 5, 99 }, 10),
                          std::runtime_error);
        REQUIRE_THROWS_AS(studentas("Jonas", "Jonaitis", { 0 }, 10),
                          std::runtime_error);
    }

    SECTION("isvestine klase meta runtime_error blogam egzaminui") {
        REQUIRE_THROWS_AS(studentas("Jonas", "Jonaitis", { 5 }, 0),
                          std::runtime_error);
        REQUIRE_THROWS_AS(studentas("Jonas", "Jonaitis", { 5 }, 11),
                          std::runtime_error);
    }

    SECTION("ribines reiksmes 1 ir 10 yra leidziamos") {
        REQUIRE_NOTHROW(studentas("Jonas", "Jonaitis", { 1, 10 }, 1));
        REQUIRE_NOTHROW(studentas("Jonas", "Jonaitis", { 1, 10 }, 10));
    }
}

// =====================================================================
// 3. RULE OF FIVE — pagrindinis testų blokas
// =====================================================================

TEST_CASE("Rule of Five: kopijavimo konstruktorius", "[rule5][kopijavimas]")
{
    studentas originalas = kurkTestini();
    originalas.calculateGalutinis();

    studentas kopija(originalas);

    SECTION("bazines klases laukai nukopijuoti") {
        // Daznausia paveldejimo klaida: pamirsus zmogus(other) kvietima,
        // vardas ir pavarde liktu tusti
        REQUIRE(kopija.vardas() == "Jonas");
        REQUIRE(kopija.pavarde() == "Jonaitis");
    }

    SECTION("isvestines klases laukai nukopijuoti") {
        REQUIRE(kopija.nd() == originalas.nd());
        REQUIRE(kopija.egzaminas() == originalas.egzaminas());
        REQUIRE(kopija.galutinisVid() == Approx(originalas.galutinisVid()));
        REQUIRE(kopija.galutinisMed() == Approx(originalas.galutinisMed()));
    }

    SECTION("kopija yra GILI (deep copy) — objektai nepriklausomi") {
        kopija.setVardas("Pakeistas");
        REQUIRE(originalas.vardas() == "Jonas");

        kopija.addPazymys(1);
        REQUIRE(originalas.nd().size() == 5);
        REQUIRE(kopija.nd().size() == 6);
    }
}

TEST_CASE("Rule of Five: kopijavimo priskyrimo operatorius", "[rule5][kopijavimas]")
{
    studentas a = kurkTestini();
    a.calculateGalutinis();

    SECTION("priskyrimas nukopijuoja visus laukus") {
        studentas b;
        b = a;
        REQUIRE(b.vardas() == a.vardas());
        REQUIRE(b.pavarde() == a.pavarde());
        REQUIRE(b.nd() == a.nd());
        REQUIRE(b.galutinisVid() == Approx(a.galutinisVid()));
    }

    SECTION("priskyrimas sau nesugadina objekto") {
        // Be patikros this == &other objektas galetu buti sugadintas
        a = a;
        REQUIRE(a.vardas() == "Jonas");
        REQUIRE(a.nd().size() == 5);
        REQUIRE(a.egzaminas() == 10);
    }

    SECTION("grandinis priskyrimas veikia") {
        // Reikalauja, kad operator= grazintu *this
        studentas b, c;
        c = b = a;
        REQUIRE(b.vardas() == "Jonas");
        REQUIRE(c.vardas() == "Jonas");
        REQUIRE(c.nd() == a.nd());
    }

    SECTION("priskyrimas perrasos senus duomenis") {
        studentas b("Senas", "Senaitis", { 1, 1, 1 }, 1);
        b = a;
        REQUIRE(b.vardas() == "Jonas");
        REQUIRE(b.nd().size() == 5);
        REQUIRE(b.egzaminas() == 10);
    }
}

TEST_CASE("Rule of Five: perkelimo konstruktorius", "[rule5][perkelimas]")
{
    studentas saltinis = kurkTestini();
    saltinis.calculateGalutinis();
    const double tiketinasVid = saltinis.galutinisVid();

    studentas tikslas(std::move(saltinis));

    SECTION("bazines klases laukai perkelti") {
        // Reikalauja zmogus(std::move(other)) — be std::move butu
        // iskviestas KOPIJAVIMO, ne perkelimo konstruktorius
        REQUIRE(tikslas.vardas() == "Jonas");
        REQUIRE(tikslas.pavarde() == "Jonaitis");
    }

    SECTION("isvestines klases laukai perkelti") {
        REQUIRE(tikslas.nd().size() == 5);
        REQUIRE(tikslas.egzaminas() == 10);
        REQUIRE(tikslas.galutinisVid() == Approx(tiketinasVid));
    }

    SECTION("saltinis lieka galiojancioje tuscioje busenoje") {
        REQUIRE(saltinis.vardas().empty());
        REQUIRE(saltinis.pavarde().empty());
        REQUIRE(saltinis.nd().empty());
        REQUIRE(saltinis.egzaminas() == 0);
        REQUIRE_FALSE(static_cast<bool>(saltinis));
    }

    SECTION("istustinta saltini saugu naudoti is naujo") {
        saltinis.setVardas("Naujas");
        saltinis.setPavarde("Naujaitis");
        saltinis.setEgzaminas(5);
        REQUIRE(saltinis.vardas() == "Naujas");
        REQUIRE(static_cast<bool>(saltinis));
    }
}

TEST_CASE("Rule of Five: perkelimo priskyrimo operatorius", "[rule5][perkelimas]")
{
    SECTION("perkelimas i esama objekta") {
        studentas saltinis = kurkTestini();
        saltinis.calculateGalutinis();
        studentas tikslas("Senas", "Senaitis", { 1, 1 }, 1);

        tikslas = std::move(saltinis);

        REQUIRE(tikslas.vardas() == "Jonas");
        REQUIRE(tikslas.nd().size() == 5);
        REQUIRE(tikslas.egzaminas() == 10);
        REQUIRE(saltinis.vardas().empty());
        REQUIRE(saltinis.nd().empty());
    }

    SECTION("perkelimo priskyrimas sau nesukelia luzimo") {
        studentas x = kurkTestini();
        x = std::move(x);
        // Standartas negarantuoja konkrecios reiksmes po self-move,
        // tik kad objektas liktu galiojancioje busenoje
        REQUIRE_NOTHROW(x.vardas());
        REQUIRE_NOTHROW(x.nd().size());
    }
}

TEST_CASE("Rule of Five: noexcept specifikatorius", "[rule5][noexcept]")
{
    // Be noexcept std::vector perskirstymo metu naudotu KOPIJAVIMO,
    // ne perkelimo konstruktoriu (std::move_if_noexcept)
    SECTION("move konstruktorius yra noexcept") {
        REQUIRE(std::is_nothrow_move_constructible<studentas>::value);
    }

    SECTION("move priskyrimas yra noexcept") {
        REQUIRE(std::is_nothrow_move_assignable<studentas>::value);
    }

    SECTION("bazine klase taip pat noexcept") {
        REQUIRE(std::is_nothrow_move_constructible<zmogus>::value);
        REQUIRE(std::is_nothrow_move_assignable<zmogus>::value);
    }
}

TEST_CASE("Rule of Five: destruktorius", "[rule5][destruktorius]")
{
    const int priesStudentu = studentas::gyvuStudentu;
    const int priesZmoniu = zmogus::gyvuZmoniu;

    SECTION("objektai sunaikinami isejus is srities") {
        {
            studentas a = kurkTestini();
            studentas b = kurkTestini();
            studentas c(a);
            REQUIRE(studentas::gyvuStudentu == priesStudentu + 3);
            REQUIRE(zmogus::gyvuZmoniu == priesZmoniu + 3);
        }
        REQUIRE(studentas::gyvuStudentu == priesStudentu);
        REQUIRE(zmogus::gyvuZmoniu == priesZmoniu);
    }

    SECTION("virtualus destruktorius naikinant per bazines klases rodykle") {
        {
            // Be virtual destruktoriaus ~studentas() NEBUTU iskviestas,
            // ir nd_ vektorius liktu neatlaisvintas
            std::unique_ptr<zmogus> p = std::make_unique<studentas>(
                "Testas", "Testaitis", Vector<int>{ 5, 5 }, 5);
            REQUIRE(studentas::gyvuStudentu == priesStudentu + 1);
        }
        REQUIRE(studentas::gyvuStudentu == priesStudentu);
        REQUIRE(zmogus::gyvuZmoniu == priesZmoniu);
    }

    SECTION("nera objektu nutekejimo naudojant konteinerius") {
        {
            std::vector<studentas> v;
            for (int i = 0; i < 10; ++i)
                v.push_back(kurkTestini());
            REQUIRE(studentas::gyvuStudentu == priesStudentu + 10);
        }
        REQUIRE(studentas::gyvuStudentu == priesStudentu);
    }
}

// =====================================================================
// 4. POLIMORFIZMAS
// =====================================================================

TEST_CASE("Polimorfizmas", "[polimorfizmas]")
{
    studentas s = kurkTestini();
    s.calculateGalutinis();

    SECTION("virtualus metodai per bazines klases nuoroda") {
        zmogus& z = s;
        REQUIRE(z.tipas() == "Studentas");
        REQUIRE(z.galutinis() == Approx(8.0));
        REQUIRE(z.vardas() == "Jonas");
        REQUIRE(z.pilnasVardas() == "Jonas Jonaitis");
    }

    SECTION("virtualus metodai per unique_ptr") {
        std::unique_ptr<zmogus> p = std::make_unique<studentas>(
            "Ona", "Onaite", Vector<int>{ 10, 10 }, 10);
        REQUIRE(p->tipas() == "Studentas");
        REQUIRE(p->pilnasVardas() == "Ona Onaite");
    }

    SECTION("dynamic_cast atgal i isvestine klase") {
        std::unique_ptr<zmogus> p = std::make_unique<studentas>(
            "Ona", "Onaite", Vector<int>{ 10, 10 }, 10);
        studentas* sp = dynamic_cast<studentas*>(p.get());
        REQUIRE(sp != nullptr);
        REQUIRE(sp->nd().size() == 2);
    }

    SECTION("polimorfinis konteineris") {
        std::vector<std::unique_ptr<zmogus>> zmones;
        zmones.push_back(std::make_unique<studentas>(
            "Pirmas", "Pirmaitis", Vector<int>{ 10, 10 }, 10));
        zmones.push_back(std::make_unique<studentas>(
            "Antras", "Antraitis", Vector<int>{ 1, 1 }, 1));

        for (auto& z : zmones)
            if (auto* sp = dynamic_cast<studentas*>(z.get()))
                sp->calculateGalutinis();

        REQUIRE(zmones[0]->galutinis() == Approx(10.0));
        REQUIRE(zmones[1]->galutinis() == Approx(1.0));
    }
}

// =====================================================================
// 5. ĮVESTIES / IŠVESTIES OPERATORIAI
// =====================================================================

TEST_CASE("Ivesties operatorius >>", "[io]")
{
    SECTION("nuskaito is istringstream") {
        std::istringstream iss("Ona Onaite 10 9 8 6");
        studentas s;
        iss >> s;
        REQUIRE(s.vardas() == "Ona");
        REQUIRE(s.pavarde() == "Onaite");
        REQUIRE(s.nd().size() == 3);
        REQUIRE(s.egzaminas() == 6);
    }

    SECTION("polimorfinis >> per zmogus nuoroda") {
        studentas s;
        zmogus& z = s;
        std::istringstream iss("Ona Onaite 9 8 7");
        iss >> z;
        REQUIRE(s.vardas() == "Ona");
        REQUIRE(s.egzaminas() == 7);
    }
}

TEST_CASE("Isvesties operatorius <<", "[io]")
{
    studentas s = kurkTestini();
    s.calculateGalutinis();

    SECTION("isveda varda, pavarde ir balus") {
        std::ostringstream oss;
        oss << s;
        const std::string rez = oss.str();
        REQUIRE(rez.find("Jonas") != std::string::npos);
        REQUIRE(rez.find("Jonaitis") != std::string::npos);
        REQUIRE(rez.find("8.00") != std::string::npos);
    }

    SECTION("polimorfinis << per zmogus nuoroda") {
        zmogus& z = s;
        std::ostringstream oss;
        oss << z;
        REQUIRE(oss.str().find("Jonas") != std::string::npos);
    }
}

TEST_CASE("Failo I/O: parseFromLine ir appendListTo", "[io][failai]")
{
    SECTION("parseFromLine nuskaito eilute") {
        studentas s;
        s.parseFromLine("Antanas Antanaitis 7 8 9 10 5", 4, 1);
        REQUIRE(s.vardas() == "Antanas");
        REQUIRE(s.pavarde() == "Antanaitis");
        REQUIRE(s.nd().size() == 4);
        REQUIRE(s.egzaminas() == 5);
    }

    SECTION("parseFromLine meta klaida blogam pazymiui") {
        studentas s;
        REQUIRE_THROWS_AS(s.parseFromLine("Vardas Pavarde 99 5", 2, 7),
                          std::runtime_error);
    }

    SECTION("parseFromLine meta klaida trukstant duomenu") {
        studentas s;
        REQUIRE_THROWS_AS(s.parseFromLine("TikVardas", 2, 3),
                          std::runtime_error);
    }

    SECTION("appendListTo suformatuoja eilute") {
        studentas s("Rasa", "Rasaite", { 6, 7, 8 }, 9);
        std::string buferis;
        s.appendListTo(buferis);
        REQUIRE(buferis.find("Rasa") != std::string::npos);
        REQUIRE(buferis.back() == '\n');
        REQUIRE(buferis.size() == 25 + 25 + 4 * 10 + 1);
    }

    SECTION("round-trip: isvesta eilute nuskaitoma atgal") {
        // Sis testas garantuoja, kad writeStudentaiListToFile sukurta
        // faila teisingai nuskaitys readStudentaiFromFile
        studentas originalas("Rasa", "Rasaite", { 6, 7, 8 }, 9);
        std::string buferis;
        originalas.appendListTo(buferis);

        studentas nuskaitytas;
        nuskaitytas.parseFromLine(buferis, 3, 1);

        REQUIRE(nuskaitytas.vardas() == originalas.vardas());
        REQUIRE(nuskaitytas.pavarde() == originalas.pavarde());
        REQUIRE(nuskaitytas.nd() == originalas.nd());
        REQUIRE(nuskaitytas.egzaminas() == originalas.egzaminas());
        REQUIRE(nuskaitytas == originalas);
    }
}

// =====================================================================
// 6. PERDENGTI OPERATORIAI
// =====================================================================

TEST_CASE("Lyginimo operatoriai", "[operatoriai]")
{
    studentas a("Jonas", "Jonaitis", { 5, 6 }, 7);
    studentas b("Jonas", "Jonaitis", { 5, 6 }, 7);
    studentas c("Petras", "Petraitis", { 5, 6 }, 7);

    SECTION("== ir !=") {
        REQUIRE(a == b);
        REQUIRE(a != c);
        REQUIRE_FALSE(a == c);
    }

    SECTION("< ir > rikiuoja pagal pavarde") {
        studentas x("Jonas", "Adamkus", { 5 }, 7);
        studentas y("Jonas", "Zukauskas", { 5 }, 7);
        REQUIRE(x < y);
        REQUIRE(y > x);
    }

    SECTION("esant vienodoms pavardems lyginama pagal varda") {
        studentas x("Antanas", "Petraitis", { 5 }, 7);
        studentas y("Zigmas", "Petraitis", { 5 }, 7);
        REQUIRE(x < y);
    }

    SECTION("veikia su std::sort") {
        std::vector<studentas> v = { c, a };
        std::sort(v.begin(), v.end());
        REQUIRE(v[0].pavarde() == "Jonaitis");
    }
}

TEST_CASE("Indeksavimo operatorius []", "[operatoriai]")
{
    studentas s("Jonas", "Jonaitis", { 3, 6, 9 }, 10);

    SECTION("grazina teisinga pazymi") {
        REQUIRE(s[0] == 3);
        REQUIRE(s[1] == 6);
        REQUIRE(s[2] == 9);
    }

    SECTION("meta out_of_range uz ribu") {
        REQUIRE_THROWS_AS(s[3], std::out_of_range);
        REQUIRE_THROWS_AS(s[99], std::out_of_range);
    }
}

// =====================================================================
// 7. SKAIČIAVIMO LOGIKA
// =====================================================================

TEST_CASE("calculateGalutinis", "[skaiciavimas]")
{
    SECTION("nelyginis ND kiekis") {
        // ND: 2,4,6,8,10 → vidurkis 6.0, mediana 6.0
        studentas s("Testas", "Testaitis", { 2, 4, 6, 8, 10 }, 5);
        s.calculateGalutinis();
        REQUIRE(s.galutinisVid() == Approx(0.4 * 6.0 + 0.6 * 5));
        REQUIRE(s.galutinisMed() == Approx(0.4 * 6.0 + 0.6 * 5));
    }

    SECTION("lyginis ND kiekis — mediana yra dvieju vidurio reiksmiu vidurkis") {
        // ND: 1,2,3,4 → vidurkis 2.5, mediana (2+3)/2 = 2.5
        studentas s("Testas", "Testaitis", { 1, 2, 3, 4 }, 10);
        s.calculateGalutinis();
        REQUIRE(s.galutinisMed() == Approx(0.4 * 2.5 + 0.6 * 10));
    }

    SECTION("vidurkis ir mediana skiriasi esant issibarsciusiems duomenims") {
        // ND: 1,1,1,1,10 → vidurkis 2.8, mediana 1.0
        studentas s("Testas", "Testaitis", { 1, 1, 1, 1, 10 }, 5);
        s.calculateGalutinis();
        REQUIRE(s.galutinisVid() == Approx(0.4 * 2.8 + 0.6 * 5));
        REQUIRE(s.galutinisMed() == Approx(0.4 * 1.0 + 0.6 * 5));
        REQUIRE(s.galutinisVid() != Approx(s.galutinisMed()));
    }

    SECTION("tuscias ND sarasas") {
        studentas s;
        s.setVardas("Tuscias");
        s.setPavarde("Tusciaitis");
        s.setEgzaminas(8);
        s.calculateGalutinis();
        REQUIRE(s.galutinisVid() == Approx(0.6 * 8));
        REQUIRE(s.galutinisMed() == Approx(0.6 * 8));
    }

    SECTION("nesurikiuoti ND — mediana skaiciuojama teisingai") {
        studentas s("Testas", "Testaitis", { 9, 1, 5 }, 10);
        s.calculateGalutinis();
        // Surikiavus: 1,5,9 → mediana 5
        REQUIRE(s.galutinisMed() == Approx(0.4 * 5.0 + 0.6 * 10));
    }
}

// =====================================================================
// 8. SET'ERIAI
// =====================================================================

TEST_CASE("Seteriai", "[seteriai]")
{
    studentas s;

    SECTION("paveldeti bazines klases seteriai") {
        s.setVardas("Naujas");
        s.setPavarde("Naujaitis");
        REQUIRE(s.vardas() == "Naujas");
        REQUIRE(s.pavarde() == "Naujaitis");
    }

    SECTION("bazines klases seteriai validuoja") {
        REQUIRE_THROWS_AS(s.setVardas(""), std::invalid_argument);
        REQUIRE_THROWS_AS(s.setPavarde(""), std::invalid_argument);
    }

    SECTION("isvestines klases seteriai") {
        s.setNd({ 5, 6, 7 });
        s.setEgzaminas(9);
        REQUIRE(s.nd().size() == 3);
        REQUIRE(s.egzaminas() == 9);

        s.addPazymys(10);
        REQUIRE(s.nd().size() == 4);
        REQUIRE(s.nd().back() == 10);
    }

    SECTION("isvestines klases seteriai validuoja") {
        REQUIRE_THROWS_AS(s.setEgzaminas(50), std::runtime_error);
        REQUIRE_THROWS_AS(s.setNd({ 5, 99 }), std::runtime_error);
        REQUIRE_THROWS_AS(s.addPazymys(0), std::runtime_error);
    }

    SECTION("isvalyk istustina abieju klasiu laukus") {
        studentas t = kurkTestini();
        t.calculateGalutinis();
        t.isvalyk();
        REQUIRE(t.vardas().empty());
        REQUIRE(t.pavarde().empty());
        REQUIRE(t.nd().empty());
        REQUIRE(t.egzaminas() == 0);
        REQUIRE(t.galutinisVid() == Approx(0.0));
    }
}

// =====================================================================
// 9. VEIKIMAS KONTEINERIUOSE
// =====================================================================

TEST_CASE("Veikimas su STL konteineriais", "[konteineriai]")
{
    SECTION("vector perskirstymas islaiko duomenis") {
        std::vector<studentas> v;
        for (int i = 0; i < 4; ++i)
            v.push_back(studentas("Vardas" + std::to_string(i),
                                  "Pavarde" + std::to_string(i),
                                  { 5, 6, 7 }, 8));
        v.reserve(100);   // priverstinis perskirstymas
        REQUIRE(v.size() == 4);
        REQUIRE(v[0].vardas() == "Vardas0");
        REQUIRE(v[3].vardas() == "Vardas3");
    }

    SECTION("vektoriaus perkelimas su std::move") {
        std::vector<studentas> v;
        v.push_back(kurkTestini());
        v.push_back(kurkTestini());

        std::vector<studentas> v2 = std::move(v);
        REQUIRE(v2.size() == 2);
        REQUIRE(v2[0].vardas() == "Jonas");
    }

    SECTION("veikia su std::list ir std::deque") {
        std::list<studentas> l;
        std::deque<studentas> d;
        l.push_back(kurkTestini());
        d.push_back(kurkTestini());
        REQUIRE(l.front().vardas() == "Jonas");
        REQUIRE(d.front().vardas() == "Jonas");
    }
}
