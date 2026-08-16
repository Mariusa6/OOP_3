#ifndef ZMOGUS_H_DEFINED
#define ZMOGUS_H_DEFINED

#include <string>
#include <iostream>
#include <stdexcept>

/**
 * @file zmogus.h
 * @brief Abstrakti bazinė klasė, aprašanti bendrus žmogaus duomenis.
 * @author Marius Augustinas
 * @date 2026
 */

 /**
  * @class zmogus
  * @brief ABSTRAKTI bazinė klasė — bendras bet kokio žmogaus aprašymas.
  *
  * Laiko vardą ir pavardę bei apibrėžia interfeisą, kurį privalo
  * realizuoti kiekviena išvestinė klasė.
  *
  * @par Kodėl klasė abstrakti
  * Klasė padaryta abstrakti **dviem nepriklausomais mechanizmais**:
  *
  * 1. **Grynai virtualūs metodai** (`= 0`) — galutinis(), print(),
  *    read(), tipas(). Kompiliatorius neleidžia kurti objektų iš klasės,
  *    kuri turi bent vieną neapibrėžtą virtualų metodą.
  * 2. **`protected` konstruktoriai** — prieinami tik išvestinėms klasėms.
  *    Tai antras apsaugos sluoksnis.
  *
  * @par Naudojimo pavyzdys
  * @code
  * // zmogus z;                          // KLAIDA: abstrakti klasė
  * // zmogus* p = new zmogus();          // KLAIDA: abstrakti klasė
  *
  * studentas s("Jonas", "Jonaitis", {5, 6, 7}, 8);   // OK
  * zmogus& z = s;                                     // OK — nuoroda
  * std::cout << z.tipas() << "\n";                    // "Studentas"
  *
  * std::unique_ptr<zmogus> p =
  *     std::make_unique<studentas>("Ona", "Onaite", {10}, 10);   // OK
  * @endcode
  *
  * @warning Destruktorius PRIVALO būti virtualus — kitaip naikinant
  *          objektą per bazinės klasės rodyklę išvestinės klasės
  *          laukai liktų neatlaisvinti.
  *
  * @see studentas
  */
class zmogus {
protected:
    std::string vardas_;    ///< Žmogaus vardas
    std::string pavarde_;   ///< Žmogaus pavardė

    /**
     * @brief Numatytasis konstruktorius.
     *
     * Sukuria objektą su tuščiu vardu ir pavarde.
     * `protected` — kviečiamas tik iš išvestinių klasių.
     */
    zmogus();

    /**
     * @brief Pilnas konstruktorius su validacija.
     *
     * @param vardas Žmogaus vardas (negali būti tuščias)
     * @param pavarde Žmogaus pavardė (negali būti tuščia)
     *
     * @throws std::invalid_argument jei vardas arba pavardė tuščia
     */
    zmogus(const std::string& vardas, const std::string& pavarde);

    /**
     * @brief Kopijavimo konstruktorius.
     *
     * Sukuria gilią kopiją — `std::string` laukai kopijuoja
     * savo vidinius buferius.
     *
     * @param other Kopijuojamas objektas
     */
    zmogus(const zmogus& other);

    /**
     * @brief Perkėlimo (move) konstruktorius.
     *
     * Perima kito objekto resursus be kopijavimo. Šaltinis paliekamas
     * galiojančioje, bet tuščioje būsenoje.
     *
     * @param other Objektas, iš kurio perkeliami duomenys
     *
     * @note `noexcept` būtinas: be jo `std::vector` perskirstymo metu
     *       naudotų kopijavimo konstruktorių (`std::move_if_noexcept`).
     */
    zmogus(zmogus&& other) noexcept;

    /**
     * @brief Kopijavimo priskyrimo operatorius.
     *
     * @param other Priskiriamas objektas
     * @return Nuoroda į šį objektą (grandininiam priskyrimui `a = b = c`)
     *
     * @note Turi apsaugą nuo priskyrimo sau (`this == &other`).
     */
    zmogus& operator=(const zmogus& other);

    /**
     * @brief Perkėlimo (move) priskyrimo operatorius.
     *
     * @param other Objektas, iš kurio perkeliami duomenys
     * @return Nuoroda į šį objektą
     */
    zmogus& operator=(zmogus&& other) noexcept;

public:
    /**
     * @brief Gyvuojančių objektų skaitiklis.
     *
     * Didinamas kiekviename konstruktoriuje, mažinamas destruktoriuje.
     * Naudojamas testuose objektų nutekėjimui aptikti.
     */
    static int gyvuZmoniu;

    /**
     * @brief Virtualus destruktorius.
     *
     * @warning `virtual` kvalifikatorius kritiškai svarbus. Be jo:
     * @code
     * zmogus* p = new studentas(...);
     * delete p;    // kviečiamas TIK ~zmogus()
     *              // studentas laukai NEATLAISVINAMI
     * @endcode
     *
     * Su `virtual` naikinimo tvarka teisinga: `~studentas()` → `~zmogus()`.
     */
    virtual ~zmogus();

    /**
     * @brief Grąžina žmogaus vardą.
     * @return Konstanti nuoroda į vardą (be kopijavimo)
     */
    inline const std::string& vardas() const { return vardas_; }

    /**
     * @brief Grąžina žmogaus pavardę.
     * @return Konstanti nuoroda į pavardę (be kopijavimo)
     */
    inline const std::string& pavarde() const { return pavarde_; }

    /**
     * @brief Grąžina pilną vardą.
     * @return Vardas ir pavardė, atskirti tarpu
     */
    inline std::string pilnasVardas() const { return vardas_ + " " + pavarde_; }

    /**
     * @brief Nustato vardą su validacija.
     * @param v Naujas vardas
     * @throws std::invalid_argument jei vardas tuščias
     */
    void setVardas(const std::string& v);

    /**
     * @brief Nustato pavardę su validacija.
     * @param p Nauja pavardė
     * @throws std::invalid_argument jei pavardė tuščia
     */
    void setPavarde(const std::string& p);

    /**
     * @brief Grąžina galutinį įvertinimą.
     *
     * Grynai virtualus — kiekvienas žmogaus tipas skaičiuoja jį savaip.
     * Studentui tai galutinis balas pagal namų darbų vidurkį ir egzaminą.
     *
     * @return Galutinis įvertinimas
     */
    virtual double galutinis() const = 0;

    /**
     * @brief Išveda objektą į srautą.
     *
     * Grynai virtualus. Realizuoja polimorfinį `operator<<`.
     *
     * @param os Išvesties srautas
     */
    virtual void print(std::ostream& os) const = 0;

    /**
     * @brief Nuskaito objektą iš srauto.
     *
     * Grynai virtualus. Realizuoja polimorfinį `operator>>`.
     *
     * @param is Įvesties srautas
     * @return Nuoroda į tą patį srautą (grandininiam skaitymui)
     */
    virtual std::istream& read(std::istream& is) = 0;

    /**
     * @brief Grąžina žmogaus tipo pavadinimą.
     *
     * Grynai virtualus. Naudojamas diagnostikai ir logams.
     *
     * @return Tipo pavadinimas, pvz. "Studentas"
     */
    virtual std::string tipas() const = 0;
};

/**
 * @brief Polimorfinis išvesties operatorius.
 *
 * Priima `zmogus&` nuorodą, bet kviečia virtualų print() metodą.
 * Todėl veikia su bet kuria išvestine klase — dabartine ir būsimomis.
 *
 * @param os Išvesties srautas
 * @param z Išvedamas objektas
 * @return Nuoroda į srautą
 *
 * @par Pavyzdys
 * @code
 * studentas s("Jonas", "Jonaitis", {5, 6}, 7);
 * s.calculateGalutinis();
 *
 * std::cout << s << "\n";           // į ekraną
 *
 * std::ofstream f("rezultatai.txt");
 * f << s << "\n";                   // į failą
 *
 * zmogus& z = s;
 * std::cout << z << "\n";           // per bazinės klasės nuorodą
 * @endcode
 */
std::ostream& operator<<(std::ostream& os, const zmogus& z);

/**
 * @brief Polimorfinis įvesties operatorius.
 *
 * Priima `zmogus&` nuorodą, bet kviečia virtualų read() metodą.
 *
 * @param is Įvesties srautas
 * @param z Objektas, į kurį nuskaitomi duomenys
 * @return Nuoroda į srautą
 *
 * @throws std::runtime_error jei duomenys netinkami
 */
std::istream& operator>>(std::istream& is, zmogus& z);

#endif // ZMOGUS_H_DEFINED