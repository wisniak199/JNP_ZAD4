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

    template<typename Kind> 
    constexpr size_t best_yumminess(const size_t max_slices, const int best_yum, const size_t best_i, const size_t i) {
        return i == max_slices + 1 ? best_i : 
            best_yumminess<Kind>(max_slices, std::max(best_yum, Kind::yumminess(i)), Kind::yumminess(i) > best_yum ? i : best_i, i + 1);
    }
}

template <typename... Kinds>
struct Pizzeria {

    //sprawdza czy Pizzeria jest wlasciwa
    template <typename... Ts>
    struct Repetition {
        static constexpr bool value = false;
    };

    template <typename T, typename... Ts>
    struct Repetition<T, Ts...> {
        static constexpr bool value = Repetition<Ts...>::value || (std::is_same<T, Ts>::value ||...);
    };

    static_assert(!Repetition<Kinds...>::value, "Indecisive pizzeria");


    template <const size_t... Slices>
    struct Pizza {
        static constexpr auto as_array() {
            return std::array<size_t, sizeof...(Slices)>{{Slices...}};
        }

        template <typename K>
        static constexpr size_t count() {
            return std::get<next_repetition<0, K, Kinds...>::value>(std::array<size_t, sizeof...(Slices)>{{Slices...}});
        }

        //najlepszy mix danej pizzy z Pizza2
        template <typename Pizza2>
        struct Mix {
            typedef Pizza<best_yumminess<Kinds>(count<Kinds>() + Pizza2::template count<Kinds>(), 0, 0, 0)...> type;
        };

        typedef Pizzeria<Kinds...>::Pizza<(2 * Slices)...> sliced_type;
        typedef Pizzeria<Kinds...> my_pizzeria;
    };

    //Struktura która tworzy pizzę.
    template <typename Kind>
    struct make_pizza {

        static_assert((std::is_same<Kind, Kinds>::value ||...), "This pizza is not available in this pizzeria");
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

template <typename Pizza1, typename Pizza2>
struct best_mix {
    static_assert(std::is_same<typename Pizza1::my_pizzeria, typename Pizza2::my_pizzeria>::value, 
            "Pizzas have to be from the same pizzeria");
    typedef typename Pizza1::template Mix<Pizza2>::type type;
};
#endif
