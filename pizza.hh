#ifndef _PIZZA_HH_
#define _PIZZA_HH_

#include <type_traits>
#include <cstddef>
#include <array>

namespace {

    //Sprawdza czy w liście typów są dwa takie które się powtarzają.
    template <typename ...Args>
    struct is_repetition {
        static bool const value = false;
    };

    template <typename T, typename U, typename ...Args>
    struct is_repetition<T, U, Args...> {
        static bool const value = std::is_same<T, U>::value ||
            is_repetition<T, Args...>::value || is_repetition<U, Args...>::value;
    };

    //Zwraca pozycję następnego wystąpienia pierwszego typu w liście typów,
    //liczoną od I. Gdy nie ma następnego wystąpienia zwraca -1.
    //np. dla <0, char, int, char> zwraca 1.
    //  szukany_typ^    0^    1^
    template <unsigned int I, typename ...Args>
    struct next_repetition {
        static int const value = -1;
    };

    template <unsigned int I, typename T, typename U, typename ...Args>
    struct next_repetition<I, T, U, Args...> {
        static int const value = std::is_same<T, U>::value ? I
            : next_repetition<I + 1, T, Args...>::value;
    };
}



//Struktura która zlicza poszczególne ilości kawałków.
template <const size_t... I>
struct Slices_counter {
    static constexpr auto array = std::array<size_t, sizeof...(I)>{{I...}};
    typedef Slices_counter<(I * 2)...> Sliced_slices_counter;
};

template <typename... Menu>
struct Pizzeria {

    //Tyle kawałków początkowo jest w jedej pizzy.
    using DefaultSlices = Slices_counter<8>;

    //Typ reprezentujący pizzę.
    template <typename Counter, typename... Types>
    struct Pizza {

        static constexpr auto slices_array = Counter::array;

        //Sprawdza ile kawałków zadanego typu ma pizza.
        template <typename Type>
        static constexpr size_t count() {
            return is_repetition<Type, Types...>::value ?
                slices_array[next_repetition<0, Type, Types...>::value] : 0;
        }

        typedef Pizza<typename Counter::Sliced_slices_counter, Types...> sliced_type;

        //Struktura pomocnicza to tworzenia tablicy rodzajów z menu.
        template <typename... Args>
        struct AsArray {
            template <typename... T>
            static constexpr auto make_array(T... args) {
                return std::array<size_t, sizeof...(T)>{{args...}};
            }
        };

        template <typename Arg, typename... Args>
        struct AsArray<Arg, Args...> {
            template <typename... T>
            static constexpr auto make_array(T...args) {
                return AsArray<Args...>::make_array(args..., count<Arg>());
            }
        };

        //Zwraca tablicę z ilościami kwałków poszczególnych rodzajów.
        static constexpr std::array<size_t, sizeof...(Menu)> as_array() {
                return AsArray<Menu...>::make_array();
        }

    };

    //Struktura która tworzy pizzę.
    template <typename Kind>
    struct make_pizza {
        typedef Pizza<DefaultSlices, Kind> type;
    };
};

#endif
