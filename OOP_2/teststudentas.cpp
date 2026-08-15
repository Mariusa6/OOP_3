#include "testStudentas.h"
#include <memory>       // unique_ptr
#include <type_traits>  // is_abstract, is_base_of

// =======================================================
// Testavimo infrastruktūra
// =======================================================

namespace {

    int testuPraejo = 0;
    int testuKrito = 0;

    void tikrink(bool salyga, const std::string& aprasymas)
    {
        if (salyga) {
            ++testuPraejo;
            std::cout << "  [ OK ]  " << aprasymas << "\n";
        }
        else {
            ++testuKrito;
            std::cout << "  [KLAIDA] " << aprasymas << "\n";
        }
    }

    void skyrius(const std::string& pavadinimas)
    {
        std::cout << "\n--- " << pavadinimas << " ---\n";
    }

    // Testinis studentas su žinomomis reikšmėmis
    // ND: 5,5,5,5,5 -> vidurkis 5.0, mediana 5.0
    // Egzaminas 10 -> galutinis = 0.4*5 + 0.6*10 = 8.0
    studentas kurkTestini()
    {
        return studentas("Jonas", "Jonaitis", { 5, 5, 5, 5, 5 }, 10);
    }

} // anoniminis namespace

// =======================================================
// Pagrindinė testavimo funkcija
// =======================================================

bool testStudentasKlase()
{
    testuPraejo = 0;
    testuKrito = 0;

    std::cout << "\n===============================================\n";
    std::cout << " KLASIU zmogus IR studentas TESTAVIMAS (v1.5)\n";
    std::cout << "===============================================\n";

    const int pradinisStudentu = studentas::gyvuStudentu;
    const int pradinisZmoniu = zmogus::gyvuZmoniu;

    // ===================================================
    // A. ABSTRAKCIOJI KLASE IR PAVELDEJIMAS (nauja v1.5)
    // ===================================================
    skyrius("A1. Klase zmogus yra ABSTRAKTI");
    {
        tikrink(std::is_abstract<zmogus>::value,
            "std::is_abstract<zmogus> = true (turi grynai virtualiu metodu)");
        tikrink(!std::is_abstract<studentas>::value,
            "std::is_abstract<studentas> = false (visi metodai realizuoti)");

        // Sie bandymai NESIKOMPILIUOTU — todel patikra atliekama statiskai:
        //     zmogus z;                    // klaida: abstrakti klase
        //     zmogus* p = new zmogus();    // klaida: abstrakti klase
        tikrink(!std::is_default_constructible<zmogus>::value,
            "zmogus objekto sukurti NEIMANOMA (is_default_constructible = false)");
        tikrink(std::is_default_constructible<studentas>::value,
            "studentas objekta sukurti galima");
    }

    skyrius("A2. Paveldejimo rysys");
    {
        tikrink((std::is_base_of<zmogus, studentas>::value),
            "studentas yra isvestine is zmogus");
        tikrink((std::is_convertible<studentas*, zmogus*>::value),
            "studentas* konvertuojasi i zmogus* (public paveldejimas)");
        tikrink(std::has_virtual_destructor<zmogus>::value,
            "zmogus turi VIRTUALU destruktoriu");
    }

    skyrius("A3. Paveldeti bazines klases metodai");
    {
        studentas s = kurkTestini();

        tikrink(s.vardas() == "Jonas", "paveldetas vardas() veikia");
        tikrink(s.pavarde() == "Jonaitis", "paveldetas pavarde() veikia");
        tikrink(s.pilnasVardas() == "Jonas Jonaitis", "paveldetas pilnasVardas() veikia");

        s.setVardas("Petras");
        tikrink(s.vardas() == "Petras", "paveldetas setVardas() veikia");

        bool metaTuscia = false;
        try { s.setPavarde(""); }
        catch (const std::invalid_argument&) { metaTuscia = true; }
        tikrink(metaTuscia, "paveldetas setPavarde() validuoja tuscia reiksme");
    }

    skyrius("A4. POLIMORFIZMAS per bazines klases rodykle");
    {
        studentas s = kurkTestini();
        s.calculateGalutinis();

        // Nuoroda i bazine klase
        zmogus& z = s;
        tikrink(z.vardas() == "Jonas", "prieiga prie vardo per zmogus& nuoroda");
        tikrink(z.tipas() == "Studentas",
            "virtualus tipas() grazina isvestines klases reiksme");
        tikrink(std::abs(z.galutinis() - 8.0) < 1e-9,
            "virtualus galutinis() = 8.0 (isvestines klases realizacija)");

        // Rodykle i bazine klase
        std::unique_ptr<zmogus> p = std::make_unique<studentas>(
            "Ona", "Onaite", Vector<int>{ 10, 10 }, 10);
        tikrink(p->tipas() == "Studentas", "virtualus tipas() per unique_ptr<zmogus>");
        tikrink(p->pilnasVardas() == "Ona Onaite", "paveldetas metodas per rodykle");

        // dynamic_cast atgal i isvestine klase
        studentas* sp = dynamic_cast<studentas*>(p.get());
        tikrink(sp != nullptr, "dynamic_cast<studentas*> pavyko");
        if (sp) tikrink(sp->nd().size() == 2, "prieiga prie isvestines klases duomenu");
    }

    skyrius("A5. VIRTUALUS destruktorius");
    {
        const int priesZmoniu = zmogus::gyvuZmoniu;
        const int priesStudentu = studentas::gyvuStudentu;

        {
            // Objektas kuriamas kaip studentas, naikinamas per zmogus rodykle.
            // Be virtual destruktoriaus studentas laukai liktu neatlaisvinti.
            std::unique_ptr<zmogus> p = std::make_unique<studentas>(
                "Testas", "Testaitis", Vector<int>{ 5, 5 }, 5);

            tikrink(zmogus::gyvuZmoniu == priesZmoniu + 1,
                "sukurus objekta zmogus skaitiklis padidejo");
            tikrink(studentas::gyvuStudentu == priesStudentu + 1,
                "sukurus objekta studentas skaitiklis padidejo");
        }

        tikrink(studentas::gyvuStudentu == priesStudentu,
            "~studentas() iskviestas naikinant per zmogus rodykle");
        tikrink(zmogus::gyvuZmoniu == priesZmoniu,
            "~zmogus() iskviestas po ~studentas()");
    }

    skyrius("A6. Polimorfiniai operatoriai << ir >>");
    {
        studentas s = kurkTestini();
        s.calculateGalutinis();

        // operator<< priima zmogus& ir kviecia virtualu print()
        zmogus& z = s;
        std::ostringstream oss;
        oss << z;
        tikrink(oss.str().find("Jonas") != std::string::npos,
            "operator<< per zmogus& nuoroda kviecia studentas::print()");

        // operator>> priima zmogus& ir kviecia virtualu read()
        studentas s2;
        zmogus& z2 = s2;
        std::istringstream iss("Ona Onaite 9 8 7");
        iss >> z2;
        tikrink(s2.vardas() == "Ona",
            "operator>> per zmogus& nuoroda kviecia studentas::read()");
        tikrink(s2.egzaminas() == 7, "nuskaityti duomenys teisingi");
    }

    // ===================================================
    // B. KONSTRUKTORIAI (patikra is v1.2)
    // ===================================================
    skyrius("B1. Numatytasis konstruktorius");
    {
        studentas s;
        tikrink(s.vardas().empty(), "vardas tuscias");
        tikrink(s.pavarde().empty(), "pavarde tuscia");
        tikrink(s.nd().empty(), "nd vektorius tuscias");
        tikrink(s.egzaminas() == 0, "egzaminas = 0");
        tikrink(s.galutinisVid() == 0.0, "galutinisVid = 0.0");
        tikrink(s.galutinisMed() == 0.0, "galutinisMed = 0.0");
        tikrink(!static_cast<bool>(s), "operator bool grazina false tusciam objektui");
    }

    skyrius("B2. Pilnas konstruktorius");
    {
        studentas s("Jonas", "Jonaitis", { 5, 5, 5, 5, 5 }, 10);
        tikrink(s.vardas() == "Jonas", "vardas issaugotas (per bazine klase)");
        tikrink(s.pavarde() == "Jonaitis", "pavarde issaugota (per bazine klase)");
        tikrink(s.nd().size() == 5, "nd dydis = 5");
        tikrink(s.egzaminas() == 10, "egzaminas = 10");
        tikrink(static_cast<bool>(s), "operator bool grazina true");
    }

    skyrius("B3. Validacija konstruktoriuje");
    {
        bool metaTusciaVarda = false;
        try { studentas s("", "Jonaitis", { 5 }, 10); }
        catch (const std::invalid_argument&) { metaTusciaVarda = true; }
        tikrink(metaTusciaVarda,
            "BAZINE klase meta invalid_argument, kai vardas tuscias");

        bool metaTusciaPavarde = false;
        try { studentas s("Jonas", "", { 5 }, 10); }
        catch (const std::invalid_argument&) { metaTusciaPavarde = true; }
        tikrink(metaTusciaPavarde,
            "BAZINE klase meta invalid_argument, kai pavarde tuscia");

        bool metaBlogaND = false;
        try { studentas s("Jonas", "Jonaitis", { 5, 99 }, 10); }
        catch (const std::runtime_error&) { metaBlogaND = true; }
        tikrink(metaBlogaND,
            "ISVESTINE klase meta runtime_error, kai ND pazymys = 99");

        bool metaBlogaEgz = false;
        try { studentas s("Jonas", "Jonaitis", { 5 }, 0); }
        catch (const std::runtime_error&) { metaBlogaEgz = true; }
        tikrink(metaBlogaEgz,
            "ISVESTINE klase meta runtime_error, kai egzaminas = 0");
    }

    skyrius("B4. Konstruktorius is srauto");
    {
        std::istringstream iss("Petras Petraitis 8 9 10 7");
        studentas s(iss);
        tikrink(s.vardas() == "Petras", "vardas nuskaitytas");
        tikrink(s.pavarde() == "Petraitis", "pavarde nuskaityta");
        tikrink(s.nd().size() == 3, "nd dydis = 3 (paskutinis skaicius = egzaminas)");
        tikrink(s.egzaminas() == 7, "egzaminas = 7");
    }

    // ===================================================
    // C. RULE OF FIVE (patikra is v1.2 su paveldejimu)
    // ===================================================
    skyrius("C1. Kopijavimo konstruktorius");
    {
        studentas originalas = kurkTestini();
        originalas.calculateGalutinis();

        studentas kopija(originalas);

        // Svarbiausia paveldejime: ar BAZINES klases laukai nukopijuoti?
        tikrink(kopija.vardas() == originalas.vardas(),
            "BAZINES klases vardas nukopijuotas (zmogus(other) kvietimas)");
        tikrink(kopija.pavarde() == originalas.pavarde(),
            "BAZINES klases pavarde nukopijuota");
        tikrink(kopija.nd() == originalas.nd(), "nd vektorius nukopijuotas");
        tikrink(kopija.egzaminas() == originalas.egzaminas(), "egzaminas nukopijuotas");
        tikrink(kopija.galutinisVid() == originalas.galutinisVid(),
            "galutinisVid nukopijuotas");

        // Deep copy patikra
        kopija.setVardas("Pakeistas");
        tikrink(originalas.vardas() == "Jonas",
            "originalas NEPAKISTA, kai keiciama kopija (deep copy)");
    }

    skyrius("C2. Kopijavimo priskyrimo operatorius");
    {
        studentas a = kurkTestini();
        a.calculateGalutinis();
        studentas b;

        b = a;

        tikrink(b.vardas() == a.vardas(),
            "BAZINES klases vardas priskirtas (zmogus::operator= kvietimas)");
        tikrink(b.nd() == a.nd(), "nd vektorius priskirtas");
        tikrink(b.galutinisVid() == a.galutinisVid(), "galutinisVid priskirtas");

        a = a;
        tikrink(a.vardas() == "Jonas" && a.nd().size() == 5,
            "priskyrimas sau (a = a) nesugadina objekto");

        studentas c, d;
        c = d = a;
        tikrink(c.vardas() == "Jonas" && d.vardas() == "Jonas",
            "grandinis priskyrimas (c = d = a) veikia");
    }

    skyrius("C3. Perkelimo (move) konstruktorius");
    {
        studentas saltinis = kurkTestini();
        saltinis.calculateGalutinis();
        const double tiketinasVid = saltinis.galutinisVid();

        studentas tikslas(std::move(saltinis));

        tikrink(tikslas.vardas() == "Jonas",
            "BAZINES klases vardas perkeltas (zmogus(std::move(other)))");
        tikrink(tikslas.nd().size() == 5, "nd vektorius perkeltas");
        tikrink(tikslas.galutinisVid() == tiketinasVid, "galutinisVid perkeltas");

        tikrink(saltinis.vardas().empty(),
            "saltinio BAZINES klases vardas tuscias po perkelimo");
        tikrink(saltinis.nd().empty(), "saltinio nd tuscias po perkelimo");
        tikrink(saltinis.egzaminas() == 0, "saltinio egzaminas = 0 po perkelimo");
        tikrink(!static_cast<bool>(saltinis), "saltinis konvertuojasi i false");
    }

    skyrius("C4. Perkelimo (move) priskyrimo operatorius");
    {
        studentas saltinis = kurkTestini();
        saltinis.calculateGalutinis();
        studentas tikslas("Senas", "Senaitis", { 1, 1 }, 1);

        tikslas = std::move(saltinis);

        tikrink(tikslas.vardas() == "Jonas",
            "BAZINES klases vardas perkeltas (zmogus::operator=(std::move))");
        tikrink(tikslas.nd().size() == 5, "nd perkeltas priskyrimu");
        tikrink(saltinis.vardas().empty(), "saltinis istustintas");

        studentas x = kurkTestini();
        x = std::move(x);
        tikrink(x.vardas() == "Jonas" || x.vardas().empty(),
            "move priskyrimas sau nesukelia luzimo");
    }

    skyrius("C5. noexcept specifikatorius");
    {
        tikrink(std::is_nothrow_move_constructible<studentas>::value,
            "studentas move konstruktorius yra noexcept");
        tikrink(std::is_nothrow_move_assignable<studentas>::value,
            "studentas move priskyrimas yra noexcept");
    }

    skyrius("C6. Destruktorius ir naikinimo tvarka");
    {
        const int priesStudentu = studentas::gyvuStudentu;
        const int priesZmoniu = zmogus::gyvuZmoniu;
        {
            studentas a = kurkTestini();
            studentas b = kurkTestini();
            studentas c(a);
            tikrink(studentas::gyvuStudentu == priesStudentu + 3,
                "sukurti 3 studentai — studentu skaitiklis padidejo 3");
            tikrink(zmogus::gyvuZmoniu == priesZmoniu + 3,
                "kartu sukurtos 3 zmogus bazines dalys");
        }
        tikrink(studentas::gyvuStudentu == priesStudentu,
            "~studentas() sumazino studentu skaitikli iki pradinio");
        tikrink(zmogus::gyvuZmoniu == priesZmoniu,
            "~zmogus() sumazino zmoniu skaitikli iki pradinio");
    }

    // ===================================================
    // D. IVESTIES / ISVESTIES OPERATORIAI (patikra is v1.2)
    // ===================================================
    skyrius("D1. Ivesties operatorius >>");
    {
        std::istringstream iss("Ona Onaite 10 9 8 6");
        studentas s;
        iss >> s;

        tikrink(s.vardas() == "Ona", "vardas nuskaitytas per >>");
        tikrink(s.pavarde() == "Onaite", "pavarde nuskaityta per >>");
        tikrink(s.nd().size() == 3, "nuskaityti 3 ND pazymiai");
        tikrink(s.egzaminas() == 6, "egzaminas = 6 (paskutinis skaicius)");
    }

    skyrius("D2. Isvesties operatorius <<");
    {
        studentas s = kurkTestini();
        s.calculateGalutinis();

        std::ostringstream oss;
        oss << s;
        const std::string rez = oss.str();

        tikrink(rez.find("Jonas") != std::string::npos, "isvestyje yra vardas");
        tikrink(rez.find("Jonaitis") != std::string::npos, "isvestyje yra pavarde");
        tikrink(rez.find("8.00") != std::string::npos,
            "isvestyje yra galutinis balas 8.00 (0.4*5 + 0.6*10)");
        tikrink(rez.size() >= 60, "isvestis suformatuota stulpeliais");
    }

    skyrius("D3. Ivestis is failo (parseFromLine)");
    {
        studentas s;
        s.parseFromLine("Antanas Antanaitis 7 8 9 10 5", 4, 1);

        tikrink(s.vardas() == "Antanas", "vardas parsintas");
        tikrink(s.pavarde() == "Antanaitis", "pavarde parsinta");
        tikrink(s.nd().size() == 4, "parsinti 4 ND pazymiai");
        tikrink(s.egzaminas() == 5, "egzaminas = 5");

        bool metaBloga = false;
        try { studentas bad; bad.parseFromLine("Vardas Pavarde 99 5", 2, 7); }
        catch (const std::runtime_error&) { metaBloga = true; }
        tikrink(metaBloga, "meta runtime_error, kai pazymys uz ribu");

        bool metaTrukstama = false;
        try { studentas bad; bad.parseFromLine("TikVardas", 2, 3); }
        catch (const std::runtime_error&) { metaTrukstama = true; }
        tikrink(metaTrukstama, "meta runtime_error, kai truksta duomenu");
    }

    skyrius("D4. Isvestis i faila (appendListTo)");
    {
        studentas s("Rasa", "Rasaite", { 6, 7, 8 }, 9);

        std::string buferis;
        s.appendListTo(buferis);

        tikrink(buferis.find("Rasa") != std::string::npos, "buferyje yra vardas");
        tikrink(buferis.find("Rasaite") != std::string::npos, "buferyje yra pavarde");
        tikrink(buferis.back() == '\n', "eilute baigiasi nauja eilute");
        tikrink(buferis.size() == 25 + 25 + 4 * 10 + 1,
            "eilutes ilgis atitinka formatavima (25+25+4*10+1)");

        studentas s2;
        s2.parseFromLine(buferis, 3, 1);
        tikrink(s2.vardas() == s.vardas() && s2.nd() == s.nd() &&
            s2.egzaminas() == s.egzaminas(),
            "isvesta eilute korektiskai nuskaitoma atgal (round-trip)");
    }

    // ===================================================
    // E. PAPILDOMI OPERATORIAI (patikra is v1.2)
    // ===================================================
    skyrius("E1. Lyginimo operatoriai == ir !=");
    {
        studentas a("Jonas", "Jonaitis", { 5, 6 }, 7);
        studentas b("Jonas", "Jonaitis", { 5, 6 }, 7);
        studentas c("Petras", "Petraitis", { 5, 6 }, 7);

        tikrink(a == b, "vienodi objektai lygus (==)");
        tikrink(a != c, "skirtingi objektai nelygus (!=)");
        tikrink(!(a == c), "== grazina false skirtingiems");
    }

    skyrius("E2. Rikiavimo operatoriai < ir >");
    {
        studentas a("Jonas", "Adamkus", { 5 }, 7);
        studentas b("Jonas", "Zukauskas", { 5 }, 7);

        tikrink(a < b, "Adamkus < Zukauskas (pagal pavarde)");
        tikrink(b > a, "Zukauskas > Adamkus");

        studentas c("Antanas", "Petraitis", { 5 }, 7);
        studentas d("Zigmas", "Petraitis", { 5 }, 7);
        tikrink(c < d, "esant vienodoms pavardems lyginama pagal varda");

        Konteineris<studentas> v = { b, a };
        std::sort(v.begin(), v.end());
        tikrink(v[0].pavarde() == "Adamkus", "std::sort su operator< veikia");
    }

    skyrius("E3. Indeksavimo operatorius []");
    {
        studentas s("Jonas", "Jonaitis", { 3, 6, 9 }, 10);

        tikrink(s[0] == 3, "s[0] = 3");
        tikrink(s[1] == 6, "s[1] = 6");
        tikrink(s[2] == 9, "s[2] = 9");

        bool metaUzRibu = false;
        try { int x = s[99]; (void)x; }
        catch (const std::out_of_range&) { metaUzRibu = true; }
        tikrink(metaUzRibu, "meta out_of_range, kai indeksas uz ribu");
    }

    skyrius("E4. Skaiciavimo metodas calculateGalutinis");
    {
        // ND: 2,4,6,8,10 -> vidurkis 6.0, mediana 6.0
        studentas s("Testas", "Testaitis", { 2, 4, 6, 8, 10 }, 5);
        s.calculateGalutinis();

        tikrink(std::abs(s.galutinisVid() - 5.4) < 1e-9,
            "galutinisVid = 5.4 (0.4*6.0 + 0.6*5)");
        tikrink(std::abs(s.galutinisMed() - 5.4) < 1e-9,
            "galutinisMed = 5.4 (mediana 6.0)");

        // Lyginis medianos atvejis: 1,2,3,4 -> (2+3)/2 = 2.5
        studentas s2("Testas", "Testaitis", { 1, 2, 3, 4 }, 10);
        s2.calculateGalutinis();
        tikrink(std::abs(s2.galutinisMed() - (0.4 * 2.5 + 0.6 * 10)) < 1e-9,
            "mediana lyginiam kiekiui = (2+3)/2 = 2.5");

        // Tuscias ND sarasas
        studentas s3;
        s3.setVardas("Tuscias");
        s3.setPavarde("Tusciaitis");
        s3.setEgzaminas(8);
        s3.calculateGalutinis();
        tikrink(std::abs(s3.galutinisVid() - 4.8) < 1e-9,
            "tuscias ND sarasas: galutinis = 0.6*8 = 4.8");
    }

    skyrius("E5. Set'eriai ir isvalyk");
    {
        studentas s;
        s.setVardas("Naujas");          // paveldetas is zmogus
        s.setPavarde("Naujaitis");      // paveldetas is zmogus
        s.setNd({ 5, 6, 7 });           // studentas metodas
        s.setEgzaminas(9);              // studentas metodas

        tikrink(s.vardas() == "Naujas", "paveldetas setVardas veikia");
        tikrink(s.nd().size() == 3, "setNd veikia");
        tikrink(s.egzaminas() == 9, "setEgzaminas veikia");

        s.addPazymys(10);
        tikrink(s.nd().size() == 4 && s.nd().back() == 10, "addPazymys veikia");

        bool metaBloga = false;
        try { s.setEgzaminas(50); }
        catch (const std::runtime_error&) { metaBloga = true; }
        tikrink(metaBloga, "setEgzaminas meta klaida, kai reiksme uz ribu");

        s.isvalyk();
        tikrink(s.vardas().empty() && s.nd().empty() && s.egzaminas() == 0,
            "isvalyk() istustina ir bazines, ir isvestines klases laukus");
    }

    skyrius("E6. Veikimas konteineriuose (move semantika)");
    {
        Konteineris<studentas> v;
        v.reserve(4);
        for (int i = 0; i < 4; ++i)
            v.push_back(studentas("Vardas" + std::to_string(i),
                "Pavarde" + std::to_string(i),
                { 5, 6, 7 }, 8));

        tikrink(v.size() == 4, "4 objektai idėti i vektoriu");

        v.reserve(100);
        tikrink(v[0].vardas() == "Vardas0" && v[3].vardas() == "Vardas3",
            "duomenys islieka po vektoriaus perskirstymo");

        Konteineris<studentas> v2 = std::move(v);
        tikrink(v2.size() == 4, "vektorius perkeltas su std::move");
        tikrink(v2[2].vardas() == "Vardas2", "duomenys teisingi po perkelimo");
    }

    skyrius("E7. Polimorfinis konteineris (nauja v1.5)");
    {
        // Konteineris bazines klases rodykliu — leidzia laikyti
        // skirtingu isvestiniu klasiu objektus vienoje vietoje
        Konteineris<std::unique_ptr<zmogus>> zmones;
        zmones.push_back(std::make_unique<studentas>(
            "Pirmas", "Pirmaitis", Vector<int>{ 10, 10 }, 10));
        zmones.push_back(std::make_unique<studentas>(
            "Antras", "Antraitis", Vector<int>{ 1, 1 }, 1));

        for (auto& z : zmones) {
            studentas* sp = dynamic_cast<studentas*>(z.get());
            if (sp) sp->calculateGalutinis();
        }

        tikrink(zmones.size() == 2, "polimorfiniame konteineryje 2 objektai");
        tikrink(zmones[0]->tipas() == "Studentas", "virtualus tipas() veikia");
        tikrink(std::abs(zmones[0]->galutinis() - 10.0) < 1e-9,
            "pirmojo galutinis() = 10.0");
        tikrink(std::abs(zmones[1]->galutinis() - 1.0) < 1e-9,
            "antrojo galutinis() = 1.0");

        // Isvedimas per polimorfini operator<<
        std::ostringstream oss;
        for (const auto& z : zmones)
            oss << *z << "\n";
        tikrink(oss.str().find("Pirmas") != std::string::npos &&
            oss.str().find("Antras") != std::string::npos,
            "polimorfinis operator<< isveda visus objektus");
    }

    // ===================================================
    // Rezultatai
    // ===================================================
    const int galutinisStudentu = studentas::gyvuStudentu;
    const int galutinisZmoniu = zmogus::gyvuZmoniu;

    std::cout << "\n===============================================\n";
    std::cout << " REZULTATAI\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "  Praejo:  " << testuPraejo << "\n";
    std::cout << "  Krito:   " << testuKrito << "\n";
    std::cout << "  Is viso: " << (testuPraejo + testuKrito) << "\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "  studentas objektu nutekejimas: "
        << (galutinisStudentu == pradinisStudentu ? "OK" : "KLAIDA") << "\n";
    std::cout << "  zmogus objektu nutekejimas:    "
        << (galutinisZmoniu == pradinisZmoniu ? "OK" : "KLAIDA") << "\n";
    std::cout << "===============================================\n\n";

    return testuKrito == 0
        && galutinisStudentu == pradinisStudentu
        && galutinisZmoniu == pradinisZmoniu;
}