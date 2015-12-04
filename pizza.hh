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


template <typename... Menu>
struct Pizzeria {

    //Tyle kawałków początkowo jest w jedej pizzy.
    //Tablica reprezentuje ilość różnych smaków, pola ilość kawałków danego rodzaju..
    static constexpr size_t slices[1] = {8};

    //Typ reprezentujący pizzę.
    //Tablica z ilościami kawałków, lista typów to rodzaje pizzy.
    //na n-tym miejscu w tablicy jest ilość kawałków n-tego typu.
    template <const size_t Slices[], typename... Types>
    struct Pizza {

        //Sprawdza czy pizza ma kawałki zadanego typu.
        template <typename Type>
        static constexpr size_t count() {
            return is_repetition<Type, Types...>::value ?
                Slices[next_repetition<0, Type, Types...>::value] : 0;
        }

        //Struktura pomocnicza to tworzenia tablicy rodzajów z menu.
        template <typename... Args>
        struct AsArray {
            template <typename... T>
            static constexpr auto make_array(T... args)
                ->decltype(std::array<size_t, sizeof...(T)>{args...}) {
                return std::array<size_t, sizeof...(T)>{args...};
            }
        };

        template <typename Arg, typename... Args>
        struct AsArray<Arg, Args...> {
            template <typename... T>
            static constexpr auto make_array(T...args)
                -> decltype(AsArray<Args...>::make_array(args..., count<Arg>())) {
                return AsArray<Args...>::make_array(args..., count<Arg>());
            }
        };

        //Zwraca tablicę rodzajów pizz z menu.
        static constexpr std::array<size_t, sizeof...(Menu)> as_array() {
                return AsArray<Menu...>::make_array();
        };
    };

    template <typename Kind>
    struct make_pizza {
        typedef Pizza<slices, Kind> type;
    };
};

#endif
