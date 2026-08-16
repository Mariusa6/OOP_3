#ifndef STUDENTAS_H_DEFINED
#define STUDENTAS_H_DEFINED

#include "zmogus.h"
#include "Vector.h"

#include <string>
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <charconv>

/**
 * @file studentas.h
 * @brief Išvestinė klasė studentas — studento duomenys ir galutinių balų skaičiavimas.
 * @author Marius Augustinas
 * @date 2026
 */

 /**
  * @class studentas
  * @brief Studento duomenys — išvestinė klasė iš abstrakčios zmogus.
  *
  * Prie paveldėto vardo ir pavardės prideda namų darbų pažymius,
  * egzamino įvertinimą ir du galutinius balus (pagal vidurkį ir pagal medianą).
  *
  * @par Paveldėjimas
  * Iš zmogus paveldi:
  * - `vardas_`, `pavarde_` (`protected` laukai)
  * - vardas(), pavarde(), pilnasVardas() get'erius
  * - setVardas(), setPavarde() set'erius su validacija
  * - virtualų destruktorių
  *
  * Realizuoja visus keturis grynai virtualius bazinės klasės metodus:
  * galutinis(), print(), read(), tipas().
  *
  * @par Galutinio balo formulė
  * @code
  * galutinis = 0.4 * namų_darbų_vidurkis_arba_mediana + 0.6 * egzaminas
  * @endcode
  * Svoriai apibrėžti konstantomis #namuDarbaiSvoris ir #egzaminasSvoris.
  *
  * @par Naudojimo pavyzdys
  * @code
  * // Sukūrimas su pilnu konstruktoriumi
  * studentas s("Jonas", "Jonaitis", {5, 6, 7, 8}, 9);
  * s.calculateGalutinis();
  * std::cout << s << "\n";
  *
  * // Nuskaitymas iš srauto
  * std::istringstream iss("Ona Onaite 10 9 8 7");
  * studentas s2;
  * iss >> s2;
  *
  * // Polimorfinis naudojimas
  * std::unique_ptr<zmogus> p = std::make_unique<studentas>(
  *     "Petras", "Petraitis", Vector<int>{5, 5}, 8);
  * std::cout << p->tipas() << ": " << p->galutinis() << "\n";
  * @endcode
  *
  * @warning Galutiniai balai NĖRA skaičiuojami automatiškai kuriant objektą —
  *          po duomenų nuskaitymo ar sukūrimo reikia iškviesti
  *          calculateGalutinis(). Toks sprendimas leidžia matuoti nuskaitymo
  *          ir skaičiavimo spartą atskirai.
  *
  * @see zmogus
  */
class studentas : public zmogus {
private:
    Vector<int> nd_;       ///< Namų darbų pažymiai (kiekvienas 1–10)
    int egzaminas_;             ///< Egzamino pažymys (1–10)
    double galutinisVid_;       ///< Galutinis balas pagal namų darbų vidurkį
    double galutinisMed_;       ///< Galutinis balas pagal namų darbų medianą

    /**
     * @brief Apskaičiuoja aritmetinį vidurkį.
     * @param nd Pažymių vektorius
     * @return Vidurkis; 0.0 jei vektorius tuščias
     */
    static double vidurkis(const Vector<int>& nd);

    /**
     * @brief Apskaičiuoja medianą.
     *
     * Nelyginiam elementų kiekiui grąžina vidurinį elementą,
     * lyginiam — dviejų vidurinių aritmetinį vidurkį.
     * Įeinantis vektorius nekeičiamas (kopijuojamas ir rikiuojamas viduje).
     *
     * @param nd Pažymių vektorius (nebūtinai surikiuotas)
     * @return Mediana; 0.0 jei vektorius tuščias
     */
    static double mediana(const Vector<int>& nd);

    /**
     * @brief Patikrina, ar pažymys patenka į leistiną intervalą.
     *
     * Sąmoningai grąžina `bool`, o ne meta išimtį: klaidos pranešimo
     * formavimas (`std::string` konkatenacija) yra brangus, o su 10 mln.
     * studentų ši funkcija kviečiama ~160 mln. kartų. Pranešimas
     * formuojamas tik ten, kur išimtis tikrai metama.
     *
     * @param p Tikrinamas pažymys
     * @return `true` jei #minPazymys ≤ p ≤ #maxPazymys
     */
    static inline bool pazymysTinkamas(int p) {
        return p >= minPazymys && p <= maxPazymys;
    }

public:
    /// @name Konstantos
    /// @{
    static constexpr int minPazymys = 1;            ///< Mažiausias leistinas pažymys
    static constexpr int maxPazymys = 10;           ///< Didžiausias leistinas pažymys
    static constexpr double namuDarbaiSvoris = 0.4; ///< Namų darbų svoris galutiniame balse
    static constexpr double egzaminasSvoris = 0.6;  ///< Egzamino svoris galutiniame balse
    /// @}

    /**
     * @brief Gyvuojančių studentų skaitiklis.
     *
     * Didinamas kiekviename konstruktoriuje, mažinamas destruktoriuje.
     * Naudojamas testuose objektų nutekėjimui aptikti — atskiras nuo
     * zmogus::gyvuZmoniu, todėl galima patikrinti, ar iškviesti abu
     * destruktoriai (`~studentas()` ir `~zmogus()`).
     */
    static int gyvuStudentu;

    // =================================================================
    /// @name Konstruktoriai
    /// @{
    // =================================================================

    /**
     * @brief Numatytasis konstruktorius.
     *
     * Sukuria tuščią studentą: be vardo, pavardės ir pažymių,
     * su nuliniais galutiniais balais. Skirtas objektui, į kurį
     * duomenys bus nuskaityti vėliau (pvz. per readStudentas()).
     */
    studentas();

    /**
     * @brief Pilnas konstruktorius su validacija.
     *
     * Vardą ir pavardę validuoja bazinė klasė, pažymius — ši klasė.
     *
     * @param vardas Studento vardas (negali būti tuščias)
     * @param pavarde Studento pavardė (negali būti tuščia)
     * @param nd Namų darbų pažymiai (kiekvienas 1–10)
     * @param egzaminas Egzamino pažymys (1–10)
     *
     * @throws std::invalid_argument jei vardas arba pavardė tuščia
     * @throws std::runtime_error jei bent vienas pažymys už ribų
     *
     * @note Galutiniai balai lieka 0.0 — reikia iškviesti calculateGalutinis().
     */
    studentas(const std::string& vardas,
        const std::string& pavarde,
        const Vector<int>& nd,
        int egzaminas);

    /**
     * @brief Konstruktorius iš įvesties srauto.
     *
     * Iškart nuskaito duomenis per readStudentas().
     * Pažymėtas `explicit`, kad neįvyktų netyčinis konvertavimas
     * iš srauto į studentą.
     *
     * @param is Įvesties srautas
     * @throws std::runtime_error jei duomenys netinkami arba jų trūksta
     */
    explicit studentas(std::istream& is);

    /// @}

    // =================================================================
    /// @name Rule of Five
    ///
    /// Visos penkios funkcijos realizuotos pilnai (ne `= default`).
    /// Kiekviena **privalo** iškviesti atitinkamą bazinės klasės funkciją —
    /// be to `vardas_` ir `pavarde_` liktų tušti arba nenukopijuoti.
    /// Tai dažniausia paveldėjimo klaida, kurios kompiliatorius neaptinka.
    /// @{
    // =================================================================

    /**
     * @brief Destruktorius.
     *
     * Išvalo studento duomenis ir sumažina #gyvuStudentu skaitiklį.
     * Bazinės klasės destruktorius `~zmogus()` iškviečiamas
     * **automatiškai** po šio kūno — jo kviesti rankiniu būdu nereikia.
     *
     * @par Naikinimo tvarka
     * `~studentas()` → `~zmogus()` (atvirkštinė kūrimo tvarkai)
     */
    ~studentas() override;

    /**
     * @brief Kopijavimo konstruktorius.
     *
     * Sukuria gilią kopiją (deep copy) — `std::string` ir `std::vector`
     * laukai kopijuoja savo vidinius buferius, todėl objektai lieka
     * visiškai nepriklausomi.
     *
     * @param other Kopijuojamas studentas
     *
     * @note Member initializer list prasideda `zmogus(other)` — be šio
     *       kvietimo būtų iškviestas `zmogus()` numatytasis konstruktorius,
     *       ir vardas su pavarde liktų tušti.
     */
    studentas(const studentas& other);

    /**
     * @brief Kopijavimo priskyrimo operatorius.
     *
     * @param other Priskiriamas studentas
     * @return Nuoroda į šį objektą (grandininiam priskyrimui `a = b = c`)
     *
     * @note Turi apsaugą nuo priskyrimo sau (`this == &other`) — be jos
     *       `a = a` galėtų sugadinti duomenis.
     * @note Bazinė dalis priskiriama kvalifikuotu kvietimu
     *       `zmogus::operator=(other)`. Be kvalifikacijos kompiliatorius
     *       parinktų `studentas::operator=` ir įvyktų begalinė rekursija.
     */
    studentas& operator=(const studentas& other);

    /**
     * @brief Perkėlimo (move) konstruktorius.
     *
     * Perima kito objekto resursus be kopijavimo. Šaltinis paliekamas
     * galiojančioje, bet tuščioje būsenoje — jį saugu naikinti arba
     * naudoti iš naujo.
     *
     * @param other Studentas, iš kurio perkeliami duomenys
     *
     * @warning Bazinė dalis perkeliama `zmogus(std::move(other))`.
     *          `std::move` čia **būtinas**: `other` yra pavadintas
     *          kintamasis, todėl pats savaime lvalue — be `std::move`
     *          būtų iškviestas `zmogus` **kopijavimo**, ne perkėlimo
     *          konstruktorius.
     *
     * @note `noexcept` kritiškai svarbus spartai: `std::vector`
     *       perskirstymo metu naudoja `std::move_if_noexcept`, todėl
     *       be `noexcept` rinktųsi kopijavimo konstruktorių.
     */
    studentas(studentas&& other) noexcept;

    /**
     * @brief Perkėlimo (move) priskyrimo operatorius.
     *
     * @param other Studentas, iš kurio perkeliami duomenys
     * @return Nuoroda į šį objektą
     *
     * @note Bazinė dalis perkeliama `zmogus::operator=(std::move(other))`.
     */
    studentas& operator=(studentas&& other) noexcept;

    /// @}

    // =================================================================
    /// @name Grynai virtualių metodų realizacija
    /// @{
    // =================================================================

    /**
     * @brief Grąžina galutinį įvertinimą (bazinės klasės interfeisas).
     *
     * Studentui tai galutinis balas pagal namų darbų **vidurkį**.
     * Medianos variantas prieinamas per galutinisMed().
     *
     * @return Galutinis balas pagal vidurkį
     * @warning Grąžina 0.0, jei nebuvo iškviesta calculateGalutinis().
     */
    double galutinis() const override;

    /**
     * @brief Išveda studentą į srautą.
     *
     * Formatas: vardas (20 simbolių), pavardė (20), galutinis pagal
     * vidurkį (20), galutinis pagal medianą (20).
     * Realizuoja polimorfinį `operator<<`.
     *
     * @param os Išvesties srautas
     */
    void print(std::ostream& os) const override;

    /**
     * @brief Nuskaito studentą iš srauto.
     *
     * Realizuoja polimorfinį `operator>>`. Deleguoja į readStudentas().
     *
     * @param is Įvesties srautas
     * @return Nuoroda į tą patį srautą
     */
    std::istream& read(std::istream& is) override;

    /**
     * @brief Grąžina tipo pavadinimą.
     * @return `"Studentas"`
     */
    std::string tipas() const override;

    /// @}

    // =================================================================
    /// @name Get'eriai
    /// @{
    // =================================================================

    /**
     * @brief Grąžina namų darbų pažymius.
     * @return Konstanti nuoroda į vektorių (be kopijavimo)
     */
    inline const Vector<int>& nd() const { return nd_; }

    /// @brief Grąžina egzamino pažymį.
    inline int egzaminas() const { return egzaminas_; }

    /// @brief Grąžina galutinį balą pagal namų darbų vidurkį.
    inline double galutinisVid() const { return galutinisVid_; }

    /// @brief Grąžina galutinį balą pagal namų darbų medianą.
    inline double galutinisMed() const { return galutinisMed_; }

    /**
     * @brief Patikrina, ar objektas neturi vardo ir pavardės.
     * @return `true` jei ir vardas, ir pavardė tušti
     */
    inline bool tuscias() const { return vardas_.empty() && pavarde_.empty(); }

    /// @}

    // =================================================================
    /// @name Set'eriai
    /// @{
    // =================================================================

    /**
     * @brief Nustato namų darbų pažymius su validacija.
     * @param nd Pažymių vektorius
     * @throws std::runtime_error jei bent vienas pažymys už ribų
     */
    void setNd(const Vector<int>& nd);

    /**
     * @brief Nustato egzamino pažymį su validacija.
     * @param e Egzamino pažymys
     * @throws std::runtime_error jei pažymys už ribų
     */
    void setEgzaminas(int e);

    /**
     * @brief Prideda vieną namų darbo pažymį.
     * @param p Pridedamas pažymys
     * @throws std::runtime_error jei pažymys už ribų
     */
    void addPazymys(int p);

    /**
     * @brief Ištuština objektą.
     *
     * Išvalo **ir bazinės, ir išvestinės** klasės laukus. Vektoriaus
     * atmintis faktiškai atlaisvinama (`shrink_to_fit`).
     */
    void isvalyk();

    /// @}

    // =================================================================
    /// @name Skaičiavimas
    /// @{
    // =================================================================

    /**
     * @brief Apskaičiuoja abu galutinius balus.
     *
     * Užpildo `galutinisVid_` ir `galutinisMed_` pagal formulę:
     * @code
     * galutinisVid = 0.4 * vidurkis(nd) + 0.6 * egzaminas
     * galutinisMed = 0.4 * mediana(nd)  + 0.6 * egzaminas
     * @endcode
     *
     * Tuščiam namų darbų sąrašui vidurkis ir mediana laikomi 0.0,
     * todėl galutinis balas yra `0.6 * egzaminas`.
     *
     * @note Kviečiamas atskirai, ne konstruktoriuje — taip nuskaitymo
     *       ir skaičiavimo spartą galima matuoti nepriklausomai.
     */
    void calculateGalutinis();

    /// @}

    // =================================================================
    /// @name Įvestis
    /// @{
    // =================================================================

    /**
     * @brief Nuskaito studentą iš srauto.
     *
     * Veikia dviem režimais priklausomai nuo @p ndCount:
     * - **ndCount > 0** — nuskaito tiksliai tiek namų darbų pažymių,
     *   po jų — egzaminą. Naudojama, kai stulpelių skaičius žinomas
     *   iš failo antraštės.
     * - **ndCount == 0** — skaito skaičius iki eilutės galo, paskutinį
     *   laiko egzaminu. Naudojama interaktyviai įvesčiai.
     *
     * @param is Įvesties srautas
     * @param ndCount Namų darbų skaičius (0 = skaityti iki galo)
     * @param lineNumber Eilutės numeris klaidos pranešimui (0 = nenaudoti)
     * @return Nuoroda į tą patį srautą
     *
     * @throws std::runtime_error jei trūksta duomenų arba pažymys už ribų
     *
     * @note Failo skaitymui naudokite parseFromLine() — jis kelis kartus
     *       greitesnis, nes neturi srauto mechanizmo.
     */
    std::istream& readStudentas(std::istream& is, int ndCount = 0, int lineNumber = 0);

    /**
     * @brief Greitas parsinimas iš eilutės (failo skaitymui).
     *
     * Optimizuota alternatyva readStudentas() metodui masiniam
     * duomenų apdorojimui:
     * - **nekuriamas** `std::istringstream` objektas kiekvienai eilutei
     * - skaičiai parsinami su `std::from_chars` (C++17) — be locale,
     *   be stream sentry, be būsenos tikrinimo
     * - klaidos pranešimai formuojami tik metant išimtį
     *
     * Su 10 mln. studentų šie trys pakeitimai sutrumpina failo
     * nuskaitymą nuo ~90 s iki ~30 s.
     *
     * @param line Nuskaitoma eilutė
     * @param ndCount Namų darbų stulpelių skaičius (iš failo antraštės)
     * @param lineNumber Eilutės numeris klaidos pranešimui
     *
     * @throws std::runtime_error jei trūksta vardo, pavardės ar pažymio,
     *         arba pažymys už ribų
     *
     * @see appendListTo() — atvirkštinė operacija
     */
    void parseFromLine(const std::string& line, int ndCount, int lineNumber);

    /// @}

    // =================================================================
    /// @name Išvestis
    /// @{
    // =================================================================

    /**
     * @brief Prideda suformatuotą eilutę į buferį (failo rašymui).
     *
     * Optimizuota alternatyva `operator<<` masiniam rašymui:
     * - skaičiai formatuojami su `std::to_chars` — be `std::setw`
     *   manipuliatorių
     * - rašoma į bendrą `std::string` buferį, ne į srautą
     *
     * Kviečianti funkcija kaupia eilutes ir iškviečia vieną
     * `file.write()` kas ~1 MB. Buferis nedidėja proporcingai studentų
     * skaičiui — `clear()` išlaiko `capacity`.
     *
     * Formatas: vardas (25), pavardė (25), kiekvienas ND pažymys (10),
     * egzaminas (10), naujos eilutės simbolis.
     *
     * @param out Buferis, į kurį pridedama eilutė
     *
     * @see parseFromLine() — atvirkštinė operacija; kartu jos sudaro
     *      round-trip: išvesta eilutė nuskaitoma atgal be pakitimų
     */
    void appendListTo(std::string& out) const;

    /// @}

    // =================================================================
    /// @name Perdengti operatoriai
    ///
    /// Programoje nenaudojami, bet realizuoti būsimiems klasės
    /// naudotojams — jie leidžia naudoti studentą su STL algoritmais
    /// ir asociatyviais konteineriais.
    /// @{
    // =================================================================

    /**
     * @brief Lygina du studentus.
     *
     * Lyginami vardas, pavardė, namų darbų pažymiai ir egzaminas.
     * Galutiniai balai **nelyginami**, nes jie išvedami iš kitų laukų.
     *
     * @param other Lyginamas studentas
     * @return `true` jei visi duomenys sutampa
     */
    bool operator==(const studentas& other) const;

    /**
     * @brief Tikrina, ar studentai skiriasi.
     * @param other Lyginamas studentas
     * @return `!(*this == other)`
     */
    bool operator!=(const studentas& other) const;

    /**
     * @brief Rikiavimo operatorius.
     *
     * Lygina pagal pavardę; esant vienodoms pavardėms — pagal vardą.
     * Leidžia naudoti `std::sort` be komparatoriaus ir dėti studentus
     * į `std::set` ar `std::map`.
     *
     * @param other Lyginamas studentas
     * @return `true` jei šis studentas eina anksčiau
     */
    bool operator<(const studentas& other) const;

    /**
     * @brief Atvirkštinis rikiavimo operatorius.
     * @param other Lyginamas studentas
     * @return `other < *this`
     */
    bool operator>(const studentas& other) const;

    /**
     * @brief Prieiga prie namų darbo pažymio pagal indeksą.
     *
     * @param i Pažymio indeksas (nuo 0)
     * @return Pažymys
     * @throws std::out_of_range jei indeksas už ribų
     */
    int operator[](size_t i) const;

    /**
     * @brief Konvertavimas į `bool` — ar objektas turi duomenų.
     *
     * Pažymėtas `explicit`, kad neįvyktų netyčinis konvertavimas į `int`
     * (pvz. `s + 1` nesikompiliuos, kaip ir turi būti).
     *
     * @return `true` jei vardas ir pavardė nėra tušti
     *
     * @par Pavyzdys
     * @code
     * studentas s;
     * if (!s) std::cout << "Objektas tuscias\n";
     * @endcode
     */
    explicit operator bool() const;

    /// @}
};

// =====================================================================
/// @name Ne-nariai lyginimo funkcijos
///
/// Naudojamos `sortStudentai()` šablone kaip komparatoriai. Nėra klasės
/// nariai, nes naudoja tik viešą interfeisą — taip laikomasi principo,
/// kad klasės narių turi būti kuo mažiau.
/// @{
// =====================================================================

/**
 * @brief Lygina pagal vardą (didėjančiai).
 * @param s1 Pirmas studentas
 * @param s2 Antras studentas
 * @return `true` jei `s1` vardas abėcėliškai anksčiau
 */
bool comparePagalVarda(const studentas& s1, const studentas& s2);

/**
 * @brief Lygina pagal pavardę (didėjančiai).
 * @param s1 Pirmas studentas
 * @param s2 Antras studentas
 * @return `true` jei `s1` pavardė abėcėliškai anksčiau
 */
bool comparePagalPavarde(const studentas& s1, const studentas& s2);

/**
 * @brief Lygina pagal egzamino pažymį (mažėjančiai).
 * @param s1 Pirmas studentas
 * @param s2 Antras studentas
 * @return `true` jei `s1` egzaminas didesnis
 */
bool comparePagalEgzamina(const studentas& s1, const studentas& s2);

/**
 * @brief Lygina pagal galutinį balą su vidurkiu (mažėjančiai).
 * @param s1 Pirmas studentas
 * @param s2 Antras studentas
 * @return `true` jei `s1` galutinis balas didesnis
 */
bool comparePagalVidurki(const studentas& s1, const studentas& s2);

/**
 * @brief Lygina pagal galutinį balą su mediana (mažėjančiai).
 * @param s1 Pirmas studentas
 * @param s2 Antras studentas
 * @return `true` jei `s1` galutinis balas didesnis
 */
bool comparePagalMediana(const studentas& s1, const studentas& s2);

/// @}

#endif // STUDENTAS_H_DEFINED