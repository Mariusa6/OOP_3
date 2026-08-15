#ifndef VECTOR_H_DEFINED
#define VECTOR_H_DEFINED

#include <memory>            // allocator, allocator_traits
#include <iterator>          // reverse_iterator, iterator_traits, distance
#include <stdexcept>         // out_of_range, length_error
#include <initializer_list>  // initializer_list
#include <algorithm>         // equal, lexicographical_compare, copy, move
#include <utility>           // move, forward, swap
#include <type_traits>       // is_integral, enable_if
#include <cstddef>           // size_t, ptrdiff_t
#include <limits>            // numeric_limits

/**
 * @file Vector.h
 * @brief Nuosava std::vector alternatyva — dinaminis masyvas.
 * @author Marius Augustinas
 * @date 2025
 */

 /**
  * @class Vector
  * @brief Dinaminis masyvas — pilnavertė std::vector alternatyva.
  *
  * Realizuoja beveik visą `std::vector` interfeisą: member tipus,
  * konstruktorius, elementų prieigą, iteratorius, talpos valdymą,
  * modifikavimo operacijas ir ne-narius operatorius.
  *
  * @tparam T Elemento tipas
  * @tparam Allocator Atminties skirstyklė (pagal nutylėjimą `std::allocator<T>`)
  *
  * @par Architektūra
  * Klasė laiko tris rodykles, kaip ir tipinė `std::vector` realizacija:
  * @code
  * begin_  →  pradžia
  * end_    →  po paskutinio elemento  (size = end_ - begin_)
  * cap_    →  po paskirtos atminties  (capacity = cap_ - begin_)
  * @endcode
  *
  * Atmintis **skiriama ir konstruojama atskirai**: `allocate()` tik
  * rezervuoja baitus, o objektai kuriami `construct()` metodu. Todėl
  * `reserve(1000)` nesukuria 1000 objektų — tik paskiria vietą.
  *
  * @par Augimo strategija
  * Pritrūkus vietos, talpa **dvigubinama** (arba nustatoma į 1, jei buvo 0).
  * Toks koeficientas duoda amortizuotą O(1) `push_back()` sudėtingumą:
  * n elementų įdėjimui reikia log₂(n) perskirstymų, o bendras kopijavimo
  * darbas yra O(n).
  *
  * @par Naudojimo pavyzdys
  * @code
  * Vector<int> v = {1, 2, 3};
  * v.push_back(4);
  * v.insert(v.begin() + 1, 10);      // {1, 10, 2, 3, 4}
  *
  * for (int x : v) std::cout << x << " ";
  *
  * Vector<int> v2(v);                 // kopija
  * Vector<int> v3(std::move(v));      // perkėlimas
  * @endcode
  *
  * @note Šioje realizacijoje `Vector<bool>` **nėra specializuotas**.
  *       Standartinis `std::vector<bool>` pakuoja reikšmes į bitus, bet
  *       tai laikoma standarto klaida, nes toks konteineris nebeatitinka
  *       bendrųjų konteinerio reikalavimų. Čia `Vector<bool>` veikia
  *       kaip įprastas `bool` masyvas.
  */
template <typename T, typename Allocator = std::allocator<T>>
class Vector {
public:
    // =================================================================
    /// @name Member tipai
    /// @{
    // =================================================================

    using value_type = T;                    ///< Elemento tipas
    using allocator_type = Allocator;            ///< Skirstyklės tipas
    using size_type = std::size_t;          ///< Dydžio tipas
    using difference_type = std::ptrdiff_t;       ///< Iteratorių skirtumo tipas
    using reference = value_type&;          ///< Nuoroda į elementą
    using const_reference = const value_type&;    ///< Konstanti nuoroda
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator = T*;                   ///< Iteratorius (raw pointer)
    using const_iterator = const T*;             ///< Konstantus iteratorius
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /// @}

private:
    pointer   begin_;       ///< Rodyklė į pirmą elementą
    pointer   end_;         ///< Rodyklė po paskutinio elemento
    pointer   cap_;         ///< Rodyklė po paskirtos atminties
    Allocator alloc_;       ///< Atminties skirstyklė

    using AllocTraits = std::allocator_traits<Allocator>;

    /**
     * @brief Sunaikina visus elementus intervale [first, last).
     *
     * Kviečia destruktorius, bet **neatlaisvina** atminties.
     */
    void destroyRange(pointer first, pointer last) {
        for (pointer p = first; p != last; ++p)
            AllocTraits::destroy(alloc_, p);
    }

    /**
     * @brief Atlaisvina visą paskirtą atmintį ir sunaikina elementus.
     */
    void deallocateAll() {
        if (begin_) {
            destroyRange(begin_, end_);
            AllocTraits::deallocate(alloc_, begin_, capacity());
        }
        begin_ = end_ = cap_ = nullptr;
    }

    /**
     * @brief Apskaičiuoja naują talpą augimo metu.
     *
     * Dvigubina esamą talpą; jei ji 0 — grąžina 1. Jei reikalinga
     * talpa didesnė nei dvigubinta, grąžina reikalingą.
     *
     * @param reikia Minimali reikalinga talpa
     * @return Nauja talpa
     * @throws std::length_error jei viršijamas max_size()
     */
    size_type augimoTalpa(size_type reikia) const {
        const size_type maxSz = max_size();
        if (reikia > maxSz)
            throw std::length_error("Vector: virsytas max_size()");

        const size_type dabar = capacity();
        if (dabar >= maxSz / 2)
            return maxSz;

        const size_type dviguba = dabar == 0 ? 1 : dabar * 2;
        return dviguba < reikia ? reikia : dviguba;
    }

    /**
     * @brief Perskirsto atmintį į naują talpą.
     *
     * Elementai perkeliami (move) jei tipas turi `noexcept` move
     * konstruktorių — kitaip kopijuojami, kad būtų išlaikyta
     * stipri išimčių sauga.
     *
     * @param naujaTalpa Nauja talpa (turi būti ≥ size())
     */
    void perskirstyk(size_type naujaTalpa) {
        pointer naujas = AllocTraits::allocate(alloc_, naujaTalpa);
        const size_type senasDydis = size();

        size_type i = 0;
        try {
            for (; i < senasDydis; ++i)
                AllocTraits::construct(alloc_, naujas + i,
                    std::move_if_noexcept(begin_[i]));
        }
        catch (...) {
            for (size_type j = 0; j < i; ++j)
                AllocTraits::destroy(alloc_, naujas + j);
            AllocTraits::deallocate(alloc_, naujas, naujaTalpa);
            throw;
        }

        deallocateAll();
        begin_ = naujas;
        end_ = naujas + senasDydis;
        cap_ = naujas + naujaTalpa;
    }

public:
    // =================================================================
    /// @name Konstruktoriai ir destruktorius
    /// @{
    // =================================================================

    /**
     * @brief Numatytasis konstruktorius — sukuria tuščią vektorių.
     * @post `size() == 0 && capacity() == 0`
     */
    Vector() noexcept(noexcept(Allocator()))
        : begin_(nullptr), end_(nullptr), cap_(nullptr), alloc_() {
    }

    /**
     * @brief Konstruktorius su skirstykle.
     * @param alloc Naudojama atminties skirstyklė
     */
    explicit Vector(const Allocator& alloc) noexcept
        : begin_(nullptr), end_(nullptr), cap_(nullptr), alloc_(alloc) {
    }

    /**
     * @brief Sukuria vektorių su n kopijų reikšmės @p value.
     * @param n Elementų skaičius
     * @param value Kopijuojama reikšmė
     * @param alloc Skirstyklė
     */
    Vector(size_type n, const T& value, const Allocator& alloc = Allocator())
        : begin_(nullptr), end_(nullptr), cap_(nullptr), alloc_(alloc)
    {
        if (n == 0) return;
        begin_ = AllocTraits::allocate(alloc_, n);
        cap_ = begin_ + n;
        end_ = begin_;
        try {
            for (size_type i = 0; i < n; ++i, ++end_)
                AllocTraits::construct(alloc_, end_, value);
        }
        catch (...) { deallocateAll(); throw; }
    }

    /**
     * @brief Sukuria vektorių su n numatytai sukonstruotų elementų.
     * @param n Elementų skaičius
     * @param alloc Skirstyklė
     */
    explicit Vector(size_type n, const Allocator& alloc = Allocator())
        : begin_(nullptr), end_(nullptr), cap_(nullptr), alloc_(alloc)
    {
        if (n == 0) return;
        begin_ = AllocTraits::allocate(alloc_, n);
        cap_ = begin_ + n;
        end_ = begin_;
        try {
            for (size_type i = 0; i < n; ++i, ++end_)
                AllocTraits::construct(alloc_, end_);
        }
        catch (...) { deallocateAll(); throw; }
    }

    /**
     * @brief Sukuria vektorių iš iteratorių intervalo [first, last).
     *
     * `enable_if` sąlyga atskiria šį konstruktorių nuo
     * `Vector(size_type, const T&)` — be jos `Vector<int>(5, 10)`
     * būtų klaidingai interpretuotas kaip intervalas.
     *
     * @tparam InputIt Įvesties iteratoriaus tipas
     * @param first Intervalo pradžia
     * @param last Intervalo pabaiga
     * @param alloc Skirstyklė
     */
    template <typename InputIt,
        typename = typename std::enable_if<
        !std::is_integral<InputIt>::value>::type>
    Vector(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        : begin_(nullptr), end_(nullptr), cap_(nullptr), alloc_(alloc)
    {
        const auto n = static_cast<size_type>(std::distance(first, last));
        if (n == 0) return;
        begin_ = AllocTraits::allocate(alloc_, n);
        cap_ = begin_ + n;
        end_ = begin_;
        try {
            for (; first != last; ++first, ++end_)
                AllocTraits::construct(alloc_, end_, *first);
        }
        catch (...) { deallocateAll(); throw; }
    }

    /**
     * @brief Kopijavimo konstruktorius.
     *
     * Sukuria gilią kopiją. Naujo vektoriaus talpa lygi kopijuojamo
     * **dydžiui**, ne talpai — nekopijuojama nepanaudota atmintis.
     *
     * @param other Kopijuojamas vektorius
     */
    Vector(const Vector& other)
        : begin_(nullptr), end_(nullptr), cap_(nullptr),
        alloc_(AllocTraits::select_on_container_copy_construction(other.alloc_))
    {
        const size_type n = other.size();
        if (n == 0) return;
        begin_ = AllocTraits::allocate(alloc_, n);
        cap_ = begin_ + n;
        end_ = begin_;
        try {
            for (size_type i = 0; i < n; ++i, ++end_)
                AllocTraits::construct(alloc_, end_, other.begin_[i]);
        }
        catch (...) { deallocateAll(); throw; }
    }

    /**
     * @brief Perkėlimo (move) konstruktorius.
     *
     * Perima kito vektoriaus atmintį be kopijavimo — tik trys
     * rodyklių priskyrimai. Šaltinis paliekamas tuščias.
     *
     * @param other Vektorius, iš kurio perkeliama
     */
    Vector(Vector&& other) noexcept
        : begin_(other.begin_), end_(other.end_), cap_(other.cap_),
        alloc_(std::move(other.alloc_))
    {
        other.begin_ = other.end_ = other.cap_ = nullptr;
    }

    /**
     * @brief Konstruktorius iš inicijavimo sąrašo.
     * @param il Inicijavimo sąrašas, pvz. `{1, 2, 3}`
     * @param alloc Skirstyklė
     */
    Vector(std::initializer_list<T> il, const Allocator& alloc = Allocator())
        : Vector(il.begin(), il.end(), alloc) {
    }

    /**
     * @brief Destruktorius — sunaikina elementus ir atlaisvina atmintį.
     */
    ~Vector() { deallocateAll(); }

    /// @}

    // =================================================================
    /// @name Priskyrimas
    /// @{
    // =================================================================

    /**
     * @brief Kopijavimo priskyrimo operatorius.
     * @param other Priskiriamas vektorius
     * @return Nuoroda į šį objektą
     */
    Vector& operator=(const Vector& other) {
        if (this == &other) return *this;
        assign(other.begin(), other.end());
        return *this;
    }

    /**
     * @brief Perkėlimo priskyrimo operatorius.
     * @param other Vektorius, iš kurio perkeliama
     * @return Nuoroda į šį objektą
     */
    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) return *this;
        deallocateAll();
        begin_ = other.begin_;
        end_ = other.end_;
        cap_ = other.cap_;
        alloc_ = std::move(other.alloc_);
        other.begin_ = other.end_ = other.cap_ = nullptr;
        return *this;
    }

    /**
     * @brief Priskyrimas iš inicijavimo sąrašo.
     * @param il Inicijavimo sąrašas
     * @return Nuoroda į šį objektą
     */
    Vector& operator=(std::initializer_list<T> il) {
        assign(il.begin(), il.end());
        return *this;
    }

    /**
     * @brief Pakeičia turinį n kopijų reikšmės @p value.
     * @param n Elementų skaičius
     * @param value Kopijuojama reikšmė
     */
    void assign(size_type n, const T& value) {
        clear();
        if (n > capacity()) {
            deallocateAll();
            begin_ = AllocTraits::allocate(alloc_, n);
            cap_ = begin_ + n;
            end_ = begin_;
        }
        for (size_type i = 0; i < n; ++i, ++end_)
            AllocTraits::construct(alloc_, end_, value);
    }

    /**
     * @brief Pakeičia turinį intervalo [first, last) elementais.
     * @tparam InputIt Įvesties iteratoriaus tipas
     * @param first Intervalo pradžia
     * @param last Intervalo pabaiga
     */
    template <typename InputIt,
        typename = typename std::enable_if<
        !std::is_integral<InputIt>::value>::type>
    void assign(InputIt first, InputIt last) {
        clear();
        const auto n = static_cast<size_type>(std::distance(first, last));
        if (n > capacity()) {
            deallocateAll();
            begin_ = AllocTraits::allocate(alloc_, n);
            cap_ = begin_ + n;
            end_ = begin_;
        }
        for (; first != last; ++first, ++end_)
            AllocTraits::construct(alloc_, end_, *first);
    }

    /**
     * @brief Pakeičia turinį inicijavimo sąrašo elementais.
     * @param il Inicijavimo sąrašas
     */
    void assign(std::initializer_list<T> il) { assign(il.begin(), il.end()); }

    /**
     * @brief Grąžina naudojamą skirstyklę.
     * @return Skirstyklės kopija
     */
    allocator_type get_allocator() const noexcept { return alloc_; }

    /// @}

    // =================================================================
    /// @name Elementų prieiga
    /// @{
    // =================================================================

    /**
     * @brief Prieiga prie elemento su ribų tikrinimu.
     * @param pos Elemento indeksas
     * @return Nuoroda į elementą
     * @throws std::out_of_range jei `pos >= size()`
     */
    reference at(size_type pos) {
        if (pos >= size())
            throw std::out_of_range("Vector::at: indeksas uz ribu");
        return begin_[pos];
    }

    /// @copydoc at(size_type)
    const_reference at(size_type pos) const {
        if (pos >= size())
            throw std::out_of_range("Vector::at: indeksas uz ribu");
        return begin_[pos];
    }

    /**
     * @brief Prieiga prie elemento **be** ribų tikrinimo.
     * @param pos Elemento indeksas
     * @return Nuoroda į elementą
     * @warning Su `pos >= size()` elgesys neapibrėžtas. Ribų tikrinimui
     *          naudokite at().
     */
    reference operator[](size_type pos) { return begin_[pos]; }

    /// @copydoc operator[](size_type)
    const_reference operator[](size_type pos) const { return begin_[pos]; }

    /// @brief Grąžina pirmą elementą. @warning Tuščiam vektoriui neapibrėžta.
    reference front() { return *begin_; }
    /// @copydoc front()
    const_reference front() const { return *begin_; }

    /// @brief Grąžina paskutinį elementą. @warning Tuščiam vektoriui neapibrėžta.
    reference back() { return *(end_ - 1); }
    /// @copydoc back()
    const_reference back() const { return *(end_ - 1); }

    /**
     * @brief Grąžina rodyklę į vidinį masyvą.
     * @return Rodyklė į duomenis; `nullptr` jei tuščias
     */
    T* data() noexcept { return begin_; }
    /// @copydoc data()
    const T* data() const noexcept { return begin_; }

    /// @}

    // =================================================================
    /// @name Iteratoriai
    /// @{
    // =================================================================

    iterator begin() noexcept { return begin_; }                 ///< Pradžia
    const_iterator begin() const noexcept { return begin_; }     ///< Pradžia (const)
    const_iterator cbegin() const noexcept { return begin_; }    ///< Pradžia (const)

    iterator end() noexcept { return end_; }                     ///< Pabaiga
    const_iterator end() const noexcept { return end_; }         ///< Pabaiga (const)
    const_iterator cend() const noexcept { return end_; }        ///< Pabaiga (const)

    /// @brief Atvirkštinė pradžia (paskutinis elementas)
    reverse_iterator rbegin() noexcept { return reverse_iterator(end_); }
    /// @copydoc rbegin()
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end_); }
    /// @copydoc rbegin()
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end_); }

    /// @brief Atvirkštinė pabaiga (prieš pirmą elementą)
    reverse_iterator rend() noexcept { return reverse_iterator(begin_); }
    /// @copydoc rend()
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin_); }
    /// @copydoc rend()
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin_); }

    /// @}

    // =================================================================
    /// @name Talpa
    /// @{
    // =================================================================

    /**
     * @brief Patikrina, ar vektorius tuščias.
     * @return `true` jei `size() == 0`
     */
    bool empty() const noexcept { return begin_ == end_; }

    /**
     * @brief Grąžina elementų skaičių.
     * @return Elementų skaičius
     */
    size_type size() const noexcept {
        return static_cast<size_type>(end_ - begin_);
    }

    /**
     * @brief Grąžina didžiausią galimą elementų skaičių.
     * @return Teorinė riba
     */
    size_type max_size() const noexcept {
        return AllocTraits::max_size(alloc_);
    }

    /**
     * @brief Paskiria atmintį bent @p n elementams.
     *
     * Jei `n <= capacity()`, nieko nedaro. Elementų **nekuria** —
     * `size()` nepakinta.
     *
     * @param n Reikalinga talpa
     * @throws std::length_error jei `n > max_size()`
     *
     * @note Naudinga, kai iš anksto žinomas elementų skaičius —
     *       išvengiama daugkartinių perskirstymų.
     */
    void reserve(size_type n) {
        if (n > max_size())
            throw std::length_error("Vector::reserve: virsytas max_size()");
        if (n > capacity())
            perskirstyk(n);
    }

    /**
     * @brief Grąžina esamą talpą.
     * @return Kiek elementų telpa be perskirstymo
     */
    size_type capacity() const noexcept {
        return static_cast<size_type>(cap_ - begin_);
    }

    /**
     * @brief Sumažina talpą iki dydžio.
     *
     * Atlaisvina nepanaudotą atmintį. Jei `capacity() == size()`,
     * nieko nedaro.
     */
    void shrink_to_fit() {
        if (capacity() > size()) {
            if (empty()) deallocateAll();
            else perskirstyk(size());
        }
    }

    /// @}

    // =================================================================
    /// @name Modifikavimas
    /// @{
    // =================================================================

    /**
     * @brief Pašalina visus elementus.
     *
     * `size()` tampa 0, bet **talpa nepakinta** — atmintis lieka
     * paskirta pakartotiniam naudojimui.
     */
    void clear() noexcept {
        destroyRange(begin_, end_);
        end_ = begin_;
    }

    /**
     * @brief Įterpia elementą prieš @p pos.
     * @param pos Pozicija, prieš kurią įterpiama
     * @param value Įterpiama reikšmė
     * @return Iteratorius į įterptą elementą
     */
    iterator insert(const_iterator pos, const T& value) {
        return insertImpl(pos, 1, value);
    }

    /**
     * @brief Įterpia elementą prieš @p pos (perkėlimu).
     * @param pos Pozicija
     * @param value Įterpiama reikšmė
     * @return Iteratorius į įterptą elementą
     */
    iterator insert(const_iterator pos, T&& value) {
        const size_type idx = static_cast<size_type>(pos - begin_);
        if (size() == capacity())
            perskirstyk(augimoTalpa(size() + 1));
        pointer p = begin_ + idx;
        if (p != end_) {
            AllocTraits::construct(alloc_, end_, std::move(*(end_ - 1)));
            std::move_backward(p, end_ - 1, end_);
            *p = std::move(value);
        }
        else {
            AllocTraits::construct(alloc_, end_, std::move(value));
        }
        ++end_;
        return p;
    }

    /**
     * @brief Įterpia n kopijų reikšmės prieš @p pos.
     * @param pos Pozicija
     * @param n Kopijų skaičius
     * @param value Įterpiama reikšmė
     * @return Iteratorius į pirmą įterptą elementą
     */
    iterator insert(const_iterator pos, size_type n, const T& value) {
        return insertImpl(pos, n, value);
    }

    /**
     * @brief Įterpia intervalo [first, last) elementus prieš @p pos.
     * @tparam InputIt Įvesties iteratoriaus tipas
     * @param pos Pozicija
     * @param first Intervalo pradžia
     * @param last Intervalo pabaiga
     * @return Iteratorius į pirmą įterptą elementą
     */
    template <typename InputIt,
        typename = typename std::enable_if<
        !std::is_integral<InputIt>::value>::type>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        const size_type idx = static_cast<size_type>(pos - begin_);
        const auto n = static_cast<size_type>(std::distance(first, last));
        if (n == 0) return begin_ + idx;

        if (size() + n > capacity())
            perskirstyk(augimoTalpa(size() + n));

        pointer p = begin_ + idx;
        // Perkeliame esamus elementus į galą
        for (pointer q = end_; q != p; --q)
            AllocTraits::construct(alloc_, q + n - 1, std::move(*(q - 1)));
        // Įrašome naujus
        pointer w = p;
        for (; first != last; ++first, ++w) {
            if (w < end_) *w = *first;
            else AllocTraits::construct(alloc_, w, *first);
        }
        end_ += n;
        return begin_ + idx;
    }

    /**
     * @brief Įterpia inicijavimo sąrašo elementus prieš @p pos.
     * @param pos Pozicija
     * @param il Inicijavimo sąrašas
     * @return Iteratorius į pirmą įterptą elementą
     */
    iterator insert(const_iterator pos, std::initializer_list<T> il) {
        return insert(pos, il.begin(), il.end());
    }

    /**
     * @brief Sukuria elementą vietoje prieš @p pos.
     *
     * Elementas konstruojamas **tiesiogiai** konteinerio atmintyje —
     * be laikino objekto ir be kopijavimo.
     *
     * @tparam Args Konstruktoriaus argumentų tipai
     * @param pos Pozicija
     * @param args Argumentai, perduodami elemento konstruktoriui
     * @return Iteratorius į sukurtą elementą
     */
    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        const size_type idx = static_cast<size_type>(pos - begin_);
        if (size() == capacity())
            perskirstyk(augimoTalpa(size() + 1));
        pointer p = begin_ + idx;
        if (p != end_) {
            AllocTraits::construct(alloc_, end_, std::move(*(end_ - 1)));
            std::move_backward(p, end_ - 1, end_);
            *p = T(std::forward<Args>(args)...);
        }
        else {
            AllocTraits::construct(alloc_, end_, std::forward<Args>(args)...);
        }
        ++end_;
        return p;
    }

    /**
     * @brief Pašalina elementą pozicijoje @p pos.
     * @param pos Šalinamo elemento pozicija
     * @return Iteratorius į kitą elementą po pašalinto
     */
    iterator erase(const_iterator pos) {
        pointer p = begin_ + (pos - begin_);
        std::move(p + 1, end_, p);
        --end_;
        AllocTraits::destroy(alloc_, end_);
        return p;
    }

    /**
     * @brief Pašalina elementus intervale [first, last).
     * @param first Intervalo pradžia
     * @param last Intervalo pabaiga
     * @return Iteratorius į kitą elementą po pašalintų
     */
    iterator erase(const_iterator first, const_iterator last) {
        pointer f = begin_ + (first - begin_);
        pointer l = begin_ + (last - begin_);
        if (f == l) return f;
        pointer naujasEnd = std::move(l, end_, f);
        destroyRange(naujasEnd, end_);
        end_ = naujasEnd;
        return f;
    }

    /**
     * @brief Prideda elementą į galą.
     *
     * Amortizuotas sudėtingumas O(1). Pritrūkus vietos, talpa
     * dvigubinama.
     *
     * @param value Pridedama reikšmė
     */
    void push_back(const T& value) {
        if (end_ == cap_)
            perskirstyk(augimoTalpa(size() + 1));
        AllocTraits::construct(alloc_, end_, value);
        ++end_;
    }

    /**
     * @brief Prideda elementą į galą (perkėlimu).
     * @param value Pridedama reikšmė
     */
    void push_back(T&& value) {
        if (end_ == cap_)
            perskirstyk(augimoTalpa(size() + 1));
        AllocTraits::construct(alloc_, end_, std::move(value));
        ++end_;
    }

    /**
     * @brief Sukuria elementą vietoje gale.
     *
     * Efektyvesnis už `push_back(T(args...))` — objektas konstruojamas
     * tiesiogiai konteinerio atmintyje, be laikino objekto.
     *
     * @tparam Args Konstruktoriaus argumentų tipai
     * @param args Argumentai, perduodami elemento konstruktoriui
     * @return Nuoroda į sukurtą elementą
     */
    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (end_ == cap_)
            perskirstyk(augimoTalpa(size() + 1));
        AllocTraits::construct(alloc_, end_, std::forward<Args>(args)...);
        ++end_;
        return *(end_ - 1);
    }

    /**
     * @brief Pašalina paskutinį elementą.
     * @warning Tuščiam vektoriui elgesys neapibrėžtas.
     */
    void pop_back() {
        --end_;
        AllocTraits::destroy(alloc_, end_);
    }

    /**
     * @brief Pakeičia elementų skaičių.
     *
     * Didinant — nauji elementai konstruojami numatytai.
     * Mažinant — pertekliniai sunaikinami, talpa nepakinta.
     *
     * @param n Naujas dydis
     */
    void resize(size_type n) {
        if (n < size()) {
            destroyRange(begin_ + n, end_);
            end_ = begin_ + n;
        }
        else if (n > size()) {
            if (n > capacity()) perskirstyk(augimoTalpa(n));
            for (size_type i = size(); i < n; ++i, ++end_)
                AllocTraits::construct(alloc_, end_);
        }
    }

    /**
     * @brief Pakeičia elementų skaičių, naujus užpildant reikšme.
     * @param n Naujas dydis
     * @param value Reikšmė naujiems elementams
     */
    void resize(size_type n, const T& value) {
        if (n < size()) {
            destroyRange(begin_ + n, end_);
            end_ = begin_ + n;
        }
        else if (n > size()) {
            if (n > capacity()) perskirstyk(augimoTalpa(n));
            for (size_type i = size(); i < n; ++i, ++end_)
                AllocTraits::construct(alloc_, end_, value);
        }
    }

    /**
     * @brief Sukeičia turinį su kitu vektoriumi.
     *
     * Sudėtingumas O(1) — sukeičiamos tik rodyklės, elementai
     * nejudinami.
     *
     * @param other Vektorius, su kuriuo keičiamasi
     */
    void swap(Vector& other) noexcept {
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(cap_, other.cap_);
        std::swap(alloc_, other.alloc_);
    }

    /// @}

private:
    /**
     * @brief Bendra insert realizacija n kopijų įterpimui.
     */
    iterator insertImpl(const_iterator pos, size_type n, const T& value) {
        const size_type idx = static_cast<size_type>(pos - begin_);
        if (n == 0) return begin_ + idx;

        if (size() + n > capacity())
            perskirstyk(augimoTalpa(size() + n));

        pointer p = begin_ + idx;
        for (pointer q = end_; q != p; --q)
            AllocTraits::construct(alloc_, q + n - 1, std::move(*(q - 1)));
        for (size_type i = 0; i < n; ++i) {
            if (p + i < end_) *(p + i) = value;
            else AllocTraits::construct(alloc_, p + i, value);
        }
        end_ += n;
        return begin_ + idx;
    }
};

// =====================================================================
/// @name Ne-nariai operatoriai
/// @{
// =====================================================================

/**
 * @brief Lygina du vektorius.
 * @return `true` jei dydžiai ir visi elementai sutampa
 */
template <typename T, typename A>
bool operator==(const Vector<T, A>& a, const Vector<T, A>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

/// @brief Tikrina, ar vektoriai skiriasi.
template <typename T, typename A>
bool operator!=(const Vector<T, A>& a, const Vector<T, A>& b) {
    return !(a == b);
}

/**
 * @brief Leksikografinis palyginimas.
 * @return `true` jei @p a eina anksčiau už @p b
 */
template <typename T, typename A>
bool operator<(const Vector<T, A>& a, const Vector<T, A>& b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

/// @brief Leksikografinis palyginimas (≤).
template <typename T, typename A>
bool operator<=(const Vector<T, A>& a, const Vector<T, A>& b) { return !(b < a); }

/// @brief Leksikografinis palyginimas (>).
template <typename T, typename A>
bool operator>(const Vector<T, A>& a, const Vector<T, A>& b) { return b < a; }

/// @brief Leksikografinis palyginimas (≥).
template <typename T, typename A>
bool operator>=(const Vector<T, A>& a, const Vector<T, A>& b) { return !(a < b); }

/**
 * @brief Sukeičia dviejų vektorių turinį.
 *
 * Ne-narė versija, kad veiktų su `std::swap` per ADL.
 */
template <typename T, typename A>
void swap(Vector<T, A>& a, Vector<T, A>& b) noexcept { a.swap(b); }

/**
 * @brief Pašalina visus elementus, lygius @p value.
 * @return Pašalintų elementų skaičius
 * @note Atitinka C++20 `std::erase`.
 */
template <typename T, typename A, typename U>
typename Vector<T, A>::size_type erase(Vector<T, A>& c, const U& value) {
    auto it = std::remove(c.begin(), c.end(), value);
    const auto n = static_cast<typename Vector<T, A>::size_type>(c.end() - it);
    c.erase(it, c.end());
    return n;
}

/**
 * @brief Pašalina visus elementus, tenkinančius predikatą.
 * @return Pašalintų elementų skaičius
 * @note Atitinka C++20 `std::erase_if`.
 */
template <typename T, typename A, typename Pred>
typename Vector<T, A>::size_type erase_if(Vector<T, A>& c, Pred pred) {
    auto it = std::remove_if(c.begin(), c.end(), pred);
    const auto n = static_cast<typename Vector<T, A>::size_type>(c.end() - it);
    c.erase(it, c.end());
    return n;
}

/// @}

#endif // VECTOR_H_DEFINED