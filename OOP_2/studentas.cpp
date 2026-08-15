#include "studentas.h"

// -------------------------------------------------------
// Statinis narys
// -------------------------------------------------------
int studentas::gyvuStudentu = 0;

// -------------------------------------------------------
// Privatūs pagalbiniai metodai
// -------------------------------------------------------

double studentas::vidurkis(const Vector<int>& nd) {
    if (nd.empty())
        return 0.0;
    double sum = std::accumulate(nd.begin(), nd.end(), 0.0);
    return sum / nd.size();
}

double studentas::mediana(const Vector<int>& nd) {
    if (nd.empty())
        return 0.0;
    Vector<int> sortedNd = nd;
    std::sort(sortedNd.begin(), sortedNd.end());
    size_t size = sortedNd.size();
    if (size % 2 == 0)
        return (sortedNd[size / 2 - 1] + sortedNd[size / 2]) / 2.0;
    else
        return sortedNd[size / 2];
}

// -------------------------------------------------------
// Vidinės pagalbinės funkcijos (tik šiam failui)
// -------------------------------------------------------

namespace {

    inline std::string vietosTekstas(int lineNumber) {
        return (lineNumber > 0)
            ? "Eilutėje " + std::to_string(lineNumber)
            : std::string("Įvestyje");
    }

    inline std::string ribuKlaida(int lineNumber, const char* ko, int p) {
        return vietosTekstas(lineNumber) + " " + ko + " pažymys už ribų ("
            + std::to_string(studentas::minPazymys) + "-"
            + std::to_string(studentas::maxPazymys) + "): "
            + std::to_string(p);
    }

} // anoniminis namespace

// =======================================================
// 1. KONSTRUKTORIAI
//
// Kiekvienas kviečia atitinkamą BAZINĖS klasės konstruktorių
// per member initializer list — zmogus() arba zmogus(v, p).
// Kūrimo tvarka: zmogus konstruktorius → studentas konstruktorius
// =======================================================

studentas::studentas()
    : zmogus(),                          // bazinės klasės konstruktorius
    nd_(), egzaminas_(0),
    galutinisVid_(0.0), galutinisMed_(0.0)
{
    ++gyvuStudentu;
}

studentas::studentas(const std::string& vardas,
    const std::string& pavarde,
    const Vector<int>& nd,
    int egzaminas)
    : zmogus(vardas, pavarde),           // bazinė validuoja vardą ir pavardę
    nd_(nd), egzaminas_(egzaminas),
    galutinisVid_(0.0), galutinisMed_(0.0)
{
    // Studentui specifinė validacija
    for (int p : nd_)
        if (!pazymysTinkamas(p))
            throw std::runtime_error("Namų darbo pažymys už ribų: " + std::to_string(p));
    if (!pazymysTinkamas(egzaminas_))
        throw std::runtime_error("Egzamino pažymys už ribų: " + std::to_string(egzaminas_));

    ++gyvuStudentu;
}

studentas::studentas(std::istream& is)
    : zmogus(),
    nd_(), egzaminas_(0),
    galutinisVid_(0.0), galutinisMed_(0.0)
{
    ++gyvuStudentu;
    readStudentas(is);
}

// =======================================================
// 2. RULE OF FIVE
//
// Visos penkios funkcijos kviečia atitinkamas BAZINĖS klasės
// funkcijas — kitaip vardas_ ir pavarde_ nebūtų nukopijuoti
// arba perkelti (dažna klaida paveldėjime!).
// =======================================================

// -------------------------------------------------------
// 2.1 DESTRUKTORIUS (override)
//
// Naikinimo tvarka atvirkštinė kūrimui:
//   ~studentas() → ~zmogus()
// Bazinės klasės destruktorius kviečiamas AUTOMATIŠKAI —
// jo rankiniu būdu kviesti nereikia ir negalima.
// -------------------------------------------------------
studentas::~studentas()
{
    nd_.clear();
    nd_.shrink_to_fit();
    egzaminas_ = 0;
    galutinisVid_ = 0.0;
    galutinisMed_ = 0.0;

    --gyvuStudentu;
    // ~zmogus() iškviečiamas automatiškai po šio kūno
}

// -------------------------------------------------------
// 2.2 KOPIJAVIMO KONSTRUKTORIUS
//
// zmogus(other) — bazinės dalies kopijavimas.
// Be šio kvietimo būtų iškviestas zmogus() numatytasis
// konstruktorius, ir vardas_/pavarde_ liktų tušti.
// -------------------------------------------------------
studentas::studentas(const studentas& other)
    : zmogus(other),                     // bazinės dalies kopija
    nd_(other.nd_),
    egzaminas_(other.egzaminas_),
    galutinisVid_(other.galutinisVid_),
    galutinisMed_(other.galutinisMed_)
{
    ++gyvuStudentu;
}

// -------------------------------------------------------
// 2.3 KOPIJAVIMO PRISKYRIMO OPERATORIUS
//
// zmogus::operator=(other) — bazinės dalies priskyrimas.
// Kvalifikuotas kvietimas būtinas, nes be jo įvyktų
// begalinė rekursija (kviestų save patį).
// -------------------------------------------------------
studentas& studentas::operator=(const studentas& other)
{
    if (this == &other)                  // apsauga nuo priskyrimo sau
        return *this;

    zmogus::operator=(other);            // bazinės dalies priskyrimas

    nd_ = other.nd_;
    egzaminas_ = other.egzaminas_;
    galutinisVid_ = other.galutinisVid_;
    galutinisMed_ = other.galutinisMed_;

    return *this;
}

// -------------------------------------------------------
// 2.4 PERKĖLIMO (MOVE) KONSTRUKTORIUS
//
// std::move(other) būtinas: other yra pavadintas kintamasis,
// todėl pats savaime yra lvalue — be std::move būtų iškviestas
// zmogus KOPIJAVIMO, ne perkėlimo konstruktorius.
//
// noexcept — kad std::vector perskirstymo metu naudotų
// perkėlimą, ne kopijavimą (std::move_if_noexcept).
// -------------------------------------------------------
studentas::studentas(studentas&& other) noexcept
    : zmogus(std::move(other)),          // bazinės dalies perkėlimas
    nd_(std::move(other.nd_)),
    egzaminas_(other.egzaminas_),
    galutinisVid_(other.galutinisVid_),
    galutinisMed_(other.galutinisMed_)
{
    // Šaltinis paliekamas galiojančioje, bet tuščioje būsenoje
    other.egzaminas_ = 0;
    other.galutinisVid_ = 0.0;
    other.galutinisMed_ = 0.0;

    ++gyvuStudentu;
}

// -------------------------------------------------------
// 2.5 PERKĖLIMO (MOVE) PRISKYRIMO OPERATORIUS
// -------------------------------------------------------
studentas& studentas::operator=(studentas&& other) noexcept
{
    if (this == &other)                  // apsauga nuo priskyrimo sau
        return *this;

    zmogus::operator=(std::move(other)); // bazinės dalies perkėlimas

    nd_ = std::move(other.nd_);
    egzaminas_ = other.egzaminas_;
    galutinisVid_ = other.galutinisVid_;
    galutinisMed_ = other.galutinisMed_;

    other.egzaminas_ = 0;
    other.galutinisVid_ = 0.0;
    other.galutinisMed_ = 0.0;

    return *this;
}

// =======================================================
// 3. GRYNAI VIRTUALIŲ METODŲ REALIZACIJA (override)
// =======================================================

// -------------------------------------------------------
// galutinis — bazinės klasės reikalaujamas įvertinimas.
// Studentui tai galutinis balas pagal vidurkį.
// -------------------------------------------------------
double studentas::galutinis() const
{
    return galutinisVid_;
}

// -------------------------------------------------------
// print — realizuoja polimorfinį operator<<
// -------------------------------------------------------
void studentas::print(std::ostream& os) const
{
    os << std::left << std::setw(20) << vardas_
        << std::left << std::setw(20) << pavarde_
        << std::fixed << std::setprecision(2)
        << std::left << std::setw(20) << galutinisVid_
        << std::left << std::setw(20) << galutinisMed_;
}

// -------------------------------------------------------
// read — realizuoja polimorfinį operator>>
// -------------------------------------------------------
std::istream& studentas::read(std::istream& is)
{
    return readStudentas(is);
}

// -------------------------------------------------------
// tipas — žmogaus tipo pavadinimas
// -------------------------------------------------------
std::string studentas::tipas() const
{
    return "Studentas";
}

// =======================================================
// 4. SET'ERIAI
// =======================================================

void studentas::setNd(const Vector<int>& nd) {
    for (int p : nd)
        if (!pazymysTinkamas(p))
            throw std::runtime_error("Namų darbo pažymys už ribų: " + std::to_string(p));
    nd_ = nd;
}

void studentas::setEgzaminas(int e) {
    if (!pazymysTinkamas(e))
        throw std::runtime_error("Egzamino pažymys už ribų: " + std::to_string(e));
    egzaminas_ = e;
}

void studentas::addPazymys(int p) {
    if (!pazymysTinkamas(p))
        throw std::runtime_error("Namų darbo pažymys už ribų: " + std::to_string(p));
    nd_.push_back(p);
}

void studentas::isvalyk() {
    vardas_.clear();      // protected laukas iš zmogus
    pavarde_.clear();     // protected laukas iš zmogus
    nd_.clear();
    nd_.shrink_to_fit();
    egzaminas_ = 0;
    galutinisVid_ = 0.0;
    galutinisMed_ = 0.0;
}

// =======================================================
// 5. SKAIČIAVIMAS
// =======================================================

void studentas::calculateGalutinis() {
    galutinisVid_ = namuDarbaiSvoris * vidurkis(nd_) + egzaminasSvoris * egzaminas_;
    galutinisMed_ = namuDarbaiSvoris * mediana(nd_) + egzaminasSvoris * egzaminas_;
}

// =======================================================
// 6. ĮVESTIS
// =======================================================

// -------------------------------------------------------
// parseFromLine — GREITAS parsinimas failo skaitymui
// Nenaudoja std::istringstream, naudoja std::from_chars
// -------------------------------------------------------
void studentas::parseFromLine(const std::string& line, int ndCount, int lineNumber)
{
    const char* p = line.data();
    const char* end = p + line.size();

    auto skipSpace = [&]() {
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\r')) ++p;
        };

    // --- Vardas ---
    skipSpace();
    const char* start = p;
    while (p < end && *p != ' ' && *p != '\t' && *p != '\r') ++p;
    if (start == p)
        throw std::runtime_error(vietosTekstas(lineNumber) + " trūksta vardo.");
    vardas_.assign(start, p - start);

    // --- Pavardė ---
    skipSpace();
    start = p;
    while (p < end && *p != ' ' && *p != '\t' && *p != '\r') ++p;
    if (start == p)
        throw std::runtime_error(vietosTekstas(lineNumber) + " trūksta pavardės.");
    pavarde_.assign(start, p - start);

    // --- Pažymiai ---
    auto readInt = [&](const char* ko) -> int {
        skipSpace();
        int v = 0;
        auto res = std::from_chars(p, end, v);
        if (res.ec != std::errc())
            throw std::runtime_error(vietosTekstas(lineNumber)
                + " trūksta " + ko + " pažymio.");
        p = res.ptr;
        return v;
        };

    nd_.clear();
    nd_.reserve(ndCount);

    for (int i = 0; i < ndCount; ++i) {
        int v = readInt("namų darbo");
        if (!pazymysTinkamas(v))
            throw std::runtime_error(ribuKlaida(lineNumber, "namų darbo", v));
        nd_.push_back(v);
    }

    egzaminas_ = readInt("egzamino");
    if (!pazymysTinkamas(egzaminas_))
        throw std::runtime_error(ribuKlaida(lineNumber, "egzamino", egzaminas_));
}

// -------------------------------------------------------
// readStudentas — įvestis per std::istream
// -------------------------------------------------------
std::istream& studentas::readStudentas(std::istream& is, int ndCount, int lineNumber) {
    if (!(is >> vardas_ >> pavarde_))
        throw std::runtime_error(vietosTekstas(lineNumber)
            + " trūksta vardo arba pavardės.");

    nd_.clear();

    if (ndCount > 0) {
        nd_.reserve(ndCount);
        for (int i = 0; i < ndCount; ++i) {
            int p;
            if (!(is >> p))
                throw std::runtime_error(vietosTekstas(lineNumber)
                    + " trūksta namų darbo pažymio.");
            if (!pazymysTinkamas(p))
                throw std::runtime_error(ribuKlaida(lineNumber, "namų darbo", p));
            nd_.push_back(p);
        }
        if (!(is >> egzaminas_))
            throw std::runtime_error(vietosTekstas(lineNumber)
                + " trūksta egzamino pažymio.");
    }
    else {
        // Nežinomas skaičius — skaitome iki galo, paskutinis yra egzaminas
        int p;
        while (is >> p) {
            if (!pazymysTinkamas(p))
                throw std::runtime_error(ribuKlaida(lineNumber, "namų darbo", p));
            nd_.push_back(p);
        }
        if (nd_.empty())
            throw std::runtime_error(vietosTekstas(lineNumber)
                + " trūksta egzamino pažymio.");

        egzaminas_ = nd_.back();
        nd_.pop_back();
        is.clear();   // eilutės pabaiga nėra klaida šiame režime
    }

    if (!pazymysTinkamas(egzaminas_))
        throw std::runtime_error(ribuKlaida(lineNumber, "egzamino", egzaminas_));

    return is;
}

// =======================================================
// 7. IŠVESTIS
// =======================================================

// -------------------------------------------------------
// appendListTo — GREITAS formatavimas failo rašymui (std::to_chars)
// -------------------------------------------------------
void studentas::appendListTo(std::string& out) const
{
    out.append(vardas_);
    if (vardas_.size() < 25) out.append(25 - vardas_.size(), ' ');

    out.append(pavarde_);
    if (pavarde_.size() < 25) out.append(25 - pavarde_.size(), ' ');

    char buf[16];
    auto appendInt = [&](int v) {
        auto res = std::to_chars(buf, buf + sizeof(buf), v);
        size_t len = static_cast<size_t>(res.ptr - buf);
        if (len < 10) out.append(10 - len, ' ');
        out.append(buf, len);
        };

    for (int nd : nd_)
        appendInt(nd);
    appendInt(egzaminas_);

    out.push_back('\n');
}

// =======================================================
// 8. PERDENGTI OPERATORIAI
// =======================================================

bool studentas::operator==(const studentas& other) const {
    return vardas_ == other.vardas_
        && pavarde_ == other.pavarde_
        && nd_ == other.nd_
        && egzaminas_ == other.egzaminas_;
}

bool studentas::operator!=(const studentas& other) const {
    return !(*this == other);
}

// Rikiavimas pagal pavardę, esant vienodoms — pagal vardą
bool studentas::operator<(const studentas& other) const {
    if (pavarde_ != other.pavarde_)
        return pavarde_ < other.pavarde_;
    return vardas_ < other.vardas_;
}

bool studentas::operator>(const studentas& other) const {
    return other < *this;
}

int studentas::operator[](size_t i) const {
    if (i >= nd_.size())
        throw std::out_of_range("Namų darbo indeksas už ribų: " + std::to_string(i));
    return nd_[i];
}

studentas::operator bool() const {
    return !vardas_.empty() && !pavarde_.empty();
}

// =======================================================
// 9. NE-NARIAI LYGINIMO FUNKCIJOS
// =======================================================

bool comparePagalVarda(const studentas& s1, const studentas& s2) {
    return s1.vardas() < s2.vardas();
}

bool comparePagalPavarde(const studentas& s1, const studentas& s2) {
    return s1.pavarde() < s2.pavarde();
}

bool comparePagalEgzamina(const studentas& s1, const studentas& s2) {
    return s1.egzaminas() > s2.egzaminas();
}

bool comparePagalVidurki(const studentas& s1, const studentas& s2) {
    return s1.galutinisVid() > s2.galutinisVid();
}

bool comparePagalMediana(const studentas& s1, const studentas& s2) {
    return s1.galutinisMed() > s2.galutinisMed();
}