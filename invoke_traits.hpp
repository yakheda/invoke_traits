#pragma once
#include <type_traits>

#define _NAMESPACE_DETAIL_BEGIN namespace detail {
#define _NAMESPACE_DETAIL_END }
#define _NAMESPACE_OPERATOR_TRAITS_BEGIN namespace operator_traits {
#define _NAMESPACE_OPERATOR_TRAITS_END }

_NAMESPACE_DETAIL_BEGIN
    template <class MyVoid, class Ty, class... Args>
    constexpr bool is_callable_impl = false;
    
    template <class Ty, class... Args>
    constexpr bool is_callable_impl<std::void_t<
        decltype(std::declval<Ty>()(std::declval<Args>()...))>, Ty, Args...> = true;

    template <class MyVoid, class Ty, template<class, class...> class Trait, class... Args>
    constexpr bool has_trait_operator_impl = false;

    template <class Ty, template<class, class...> class Trait, class... Args>
    constexpr bool has_trait_operator_impl<std::void_t<typename Trait<void, Ty, Args...>::type>, Ty, Trait, Args...> = true;

    template <class Ty>
    struct alias_type // - stores an alias of type 'Ty' and is used for SFINAE
    {
        using type = Ty;
    };
_NAMESPACE_DETAIL_END

template <class Callable, class... Args>
constexpr bool is_callable_v = detail::is_callable_impl<void, Callable, Args...>;

template <class Ty, template<class, class...> class Trait, class... Args>
constexpr bool has_operator = detail::has_trait_operator_impl<void, Ty, Trait, Args...>;
// ^^^ returns true if 'Ty' has the operator 'Trait' from namespace 'operator_traits'

_NAMESPACE_DETAIL_BEGIN
    template <class SFINAETag, class Callable, class... Args>
    struct call_result_impl // - determine fallback ('Callable(Args...)' cannot be called)
    {
        static constexpr bool is_valid = false;
    };

    template <class Callable, class... Args> // vvv enable if 'Callable(Args...)' can be called successfully
    struct call_result_impl<std::enable_if_t<is_callable_v<Callable, Args...>>, Callable, Args...>
    {
        using result_type = decltype(std::declval<Callable>()(std::declval<Args>()...));

        static constexpr bool is_valid     = true;
        static constexpr bool is_nothrow   = noexcept(std::declval<Callable>()(std::declval<Args>()...));
        static constexpr size_t args_count = sizeof...(Args);
    };

    template <class SFINAETag, class Callable, class... Args>
    constexpr bool is_nothrow_callable_impl = false;

    template <class Callable, class... Args>
    constexpr bool is_nothrow_callable_impl<std::enable_if_t<is_callable_v<Callable, Args...>>,
        Callable, Args...> = call_result_impl<void, Callable, Args...>::is_nothrow;
_NAMESPACE_DETAIL_END

template <class Callable, class... Args>
using call_result_t = detail::call_result_impl<void, Callable, Args...>;

template <class Ty, template<class, class...> class Trait, class... Args>
using op_result_t = typename Trait<void, Ty, Args...>::type;
// ^^^ uses op_result_t to deduce the result type of an operation from 'operator_traits' based on the given type and arguments

template <class Callable, class... Args>
constexpr bool is_nothrow_callable_v = detail::is_nothrow_callable_impl<void, Callable, Args...>;
// ^^^ returns true if 'Callable(Args...)' can be called, and 'Callable' tagged 'noexcept'

_NAMESPACE_OPERATOR_TRAITS_BEGIN
    template <class MyVoid, class Ty, class... Args>
    struct op_functor { };

    template <class Ty, class... Args>
    struct op_functor<std::void_t<
        decltype(std::declval<Ty&>()(std::declval<Args>()...))>, 
        Ty, Args...> : detail::alias_type<decltype(std::declval<Ty>().operator()(std::declval<Args>()...))> { };
    
    template <class MyVoid, class Ty, class... Args>
    struct op_index { };

    template <class Ty, class... Args>
    struct op_index<std::void_t<
        decltype(std::declval<Ty>().operator[](std::declval<Args>()...))>,
        Ty, Args...> : detail::alias_type<decltype(std::declval<Ty>().operator[](std::declval<Args>()...))> { };

    template <class MyVoid, class First, class Second>
    struct op_equals_to { };

    template <class First, class Second>
    struct op_equals_to<std::void_t<decltype(std::declval<First>() == std::declval<Second>())>,
        First, Second> : detail::alias_type<decltype(std::declval<First>() == std::declval<Second>())> { };
_NAMESPACE_OPERATOR_TRAITS_END