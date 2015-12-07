#ifndef _PIZZA_HH_
#define _PIZZA_HH_

#include <type_traits>
#include <cstddef>
#include <array>



namespace {


/*  //Przyda się przy assercjach
    //Sprawdza czy w liście typów są dwa takie które się powtarzają.
    template <typename ...Args>
    struct is_repetition {
        static bool const value = false;
    };

    template <typename T, typename U, typename ...Args>
    struct is_repetition<T, U, Args...> {
        static bool const value = std::is_same<T, U>::value ||
            is_repetition<T, Args...>::value || is_repetition<U, Args...>::value;
    };*/

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



template <typename... Kinds>
struct Pizzeria {

    template <const size_t... Slices>
    struct Pizza {
        static constexpr auto as_array() {
            return std::array<size_t, sizeof...(Slices)>{{Slices...}};
        }

        template <typename K>
        static constexpr size_t count() {
            return std::get<next_repetition<0, K, Kinds...>::value>(std::array<size_t, sizeof...(Slices)>{{Slices...}});
        }

        typedef Pizzeria<Kinds...>::Pizza<(2 * Slices)...> sliced_type;
    };

    //Struktura która tworzy pizzę.
    template <typename Kind>
    struct make_pizza {

        //Nazwy do zmiany...
        //Struktury pomocnicze do konstruowania typu Pizza zadanego rodzaju.
        template <typename... Types>
        struct Foo{
            template <size_t... I>
            struct Bar{
                typedef Pizzeria<Kinds...>::Pizza<I...> pizza;
            };
        };

        template <typename T, typename... Types>
        struct Foo<T, Types...> {
            template <size_t... I>
            struct Bar {
                typedef typename Foo<Types...>::template Bar<I..., std::is_same<Kind, T>::value ? 8 : 0>::pizza pizza;
            };
        };

        typedef typename Foo<Kinds...>::template Bar<>::pizza type;

    };
};

#endif
