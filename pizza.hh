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

//Trzeba zmienić żeby mogły być kawałki różnych typów pizz.
template <size_t N, typename T>
struct Pizza {
    template <typename Kind>
    static constexpr size_t count() {
        return std::is_same<T, Kind>::value ? N : 0;
    }
    typedef Pizza<static_cast<size_t>(N * 2), T> sliced_type;
};


template <typename... Kinds> struct Pizzeria {

    template <typename Kind>
    struct make_pizza {
        typedef Pizza<static_cast<size_t>(8), Kind> type;
    };
};



#endif
