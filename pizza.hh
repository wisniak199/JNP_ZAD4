#ifndef _PIZZA_HH_
#define _PIZZA_HH_

#include <type_traits>
#include <cstddef>
#include <array>

namespace {

    //Sprawdza czy na liście typów są powtórzenia.
    template <typename... Ts>
    struct Repetition {
        static constexpr bool value = false;
    };

    template <typename T, typename... Ts>
    struct Repetition<T, Ts...> {
        static constexpr bool value = Repetition<Ts...>::value ||
                                   (std::is_same<T, Ts>::value ||...);
    };

    //Oblicza najlepszą smakowitość pizzy dla zadanej liczby kawałków.
    template<typename Kind>
    constexpr size_t best_yumminess(const size_t max_slices, const int best_yum,
                                    const size_t best_i, const size_t i) {
        return i == max_slices + 1 ? best_i : best_yumminess<Kind>
                    (max_slices, std::max(best_yum, Kind::yumminess(i)),
                    Kind::yumminess(i) > best_yum ? i : best_i, i + 1);
    }
}

//Typ szablonowy reprezentujący pizzerię.
template <typename... Kinds>
struct Pizzeria {

    static_assert(!Repetition<Kinds...>::value, "Indecisive pizzeria");

    private:

        //Struktura reprezentująca pizzę.
        template <const size_t... Slices>
        struct Pizza {

            template <typename K>
            static constexpr size_t count() {
                return ((std::is_same<K, Kinds>::value * Slices)+...);
            }

            static constexpr auto as_array() {
                return std::array<size_t, sizeof...(Slices)>{{Slices...}};
            }

            typedef Pizzeria<Kinds...>::Pizza<(2 * Slices)...> sliced_type;

            private:

                //Najlepszy mix danej pizzy z Pizza2.
                template <typename Pizza2>
                struct Mix {
                    typedef Pizza<best_yumminess<Kinds>(count<Kinds>()
                        + Pizza2::template count<Kinds>(), 0, 0, 0)...> type;
                };

                typedef Pizzeria<Kinds...> my_pizzeria;

                template<typename T, typename U>
                friend struct best_mix;
        };

    public:

        //Struktura która służy do tworzenia pizzy.
        template <typename Kind>
        struct make_pizza {

            static_assert((std::is_same<Kind, Kinds>::value ||...),
                    "This pizza is not available in this pizzeria");

            typedef Pizza<(std::is_same<Kind, Kinds>::value ? 8 : 0)...> type;
        };
};

//Szablon struktury określającej jaką najlepsza możliwą pizzę można złożyć z podanych.
template <typename Pizza1, typename Pizza2>
struct best_mix {

    static_assert(std::is_same<typename Pizza1::my_pizzeria,
                               typename Pizza2::my_pizzeria>::value,
                               "Pizzas have to be from the same pizzeria");

    typedef typename Pizza1::template Mix<Pizza2>::type type;
};

#endif
