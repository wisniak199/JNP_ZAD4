#ifndef _PIZZA_HH_
#define _PIZZA_HH_

#include <type_traits>
#include <utility>
#include <cstddef>

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
}


template <typename... Kinds> struct Pizzeria {

    //Sprawdza czy w menu nie ma powtarzających się pizz.
    static_assert(!is_repetition<Kinds...>::value, ""); //coś na razie nie działa.

    template <typename Kind>
    struct make_pizza {

        //Sprawdza czy zadana pizza jest w menu.
        static_assert(is_repetition<Kind, Kinds...>::value, "");

        static constexpr size_t slices[1] = {8};

        //Typ reprezentujący pizzę.
        template <const size_t Slices[], typename... Mixed>
        struct Pizza {
        };

        typedef Pizza<slices, Kind> type;

    };
};



#endif
