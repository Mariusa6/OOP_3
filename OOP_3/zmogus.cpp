#include "zmogus.h"

// -------------------------------------------------------
// Statinis narys — gyvuojančių objektų skaitiklis
// -------------------------------------------------------
int zmogus::gyvuZmoniu = 0;

// =======================================================
// Konstruktoriai (protected)
// =======================================================

zmogus::zmogus()
    : vardas_(), pavarde_()
{
    ++gyvuZmoniu;
}

zmogus::zmogus(const std::string& vardas, const std::string& pavarde)
    : vardas_(vardas), pavarde_(pavarde)
{
    if (vardas_.empty())
        throw std::invalid_argument("Vardas negali būti tuščias");
    if (pavarde_.empty())
        throw std::invalid_argument("Pavardė negali būti tuščia");

    ++gyvuZmoniu;
}

// -------------------------------------------------------
// Kopijavimo konstruktorius
// -------------------------------------------------------
zmogus::zmogus(const zmogus& other)
    : vardas_(other.vardas_), pavarde_(other.pavarde_)
{
    ++gyvuZmoniu;
}

// -------------------------------------------------------
// Perkėlimo (move) konstruktorius
// noexcept — kad std::vector naudotų perkėlimą, ne kopijavimą
// -------------------------------------------------------
zmogus::zmogus(zmogus&& other) noexcept
    : vardas_(std::move(other.vardas_)), pavarde_(std::move(other.pavarde_))
{
    ++gyvuZmoniu;
}

// -------------------------------------------------------
// Kopijavimo priskyrimo operatorius
// -------------------------------------------------------
zmogus& zmogus::operator=(const zmogus& other)
{
    if (this == &other)       // apsauga nuo priskyrimo sau
        return *this;

    vardas_ = other.vardas_;
    pavarde_ = other.pavarde_;

    return *this;
}

// -------------------------------------------------------
// Perkėlimo (move) priskyrimo operatorius
// -------------------------------------------------------
zmogus& zmogus::operator=(zmogus&& other) noexcept
{
    if (this == &other)       // apsauga nuo priskyrimo sau
        return *this;

    vardas_ = std::move(other.vardas_);
    pavarde_ = std::move(other.pavarde_);

    return *this;
}

// =======================================================
// VIRTUALUS destruktorius
//
// Kviečiamas PO išvestinės klasės destruktoriaus.
// Naikinimo tvarka: ~studentas() → ~zmogus()
// =======================================================
zmogus::~zmogus()
{
    vardas_.clear();
    pavarde_.clear();

    --gyvuZmoniu;
}

// =======================================================
// Set'eriai su validacija
// =======================================================

void zmogus::setVardas(const std::string& v)
{
    if (v.empty())
        throw std::invalid_argument("Vardas negali būti tuščias");
    vardas_ = v;
}

void zmogus::setPavarde(const std::string& p)
{
    if (p.empty())
        throw std::invalid_argument("Pavardė negali būti tuščia");
    pavarde_ = p;
}

// =======================================================
// POLIMORFINIAI operatoriai
// =======================================================

std::ostream& operator<<(std::ostream& os, const zmogus& z)
{
    z.print(os);              // virtualus kvietimas
    return os;
}

std::istream& operator>>(std::istream& is, zmogus& z)
{
    return z.read(is);        // virtualus kvietimas
}