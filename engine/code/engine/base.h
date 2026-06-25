#pragma once

#include <stddef.h>

#include <fmt/core.h>
#include <fmt/color.h>

#include <string_view>
#include <span>

#define PP_CONCAT(a, b)         PP_CONCAT_IMPL_0(a, b)
#define PP_CONCAT_IMPL_0(a, b)  PP_CONCAT_IMPL_1(a, b)
#define PP_CONCAT_IMPL_1(a, b)  a##b

#define PP_TOSTRING(a)          PP_TOSTRING_IMPL_0(a)
#define PP_TOSTRING_IMPL_0(a)   #a

template <typename Func>
struct DeferHelperObject_
{
    Func func;
    DeferHelperObject_(Func fn) : func(fn) {}
    ~DeferHelperObject_() { func(); }
};

#define defer DeferHelperObject_ PP_CONCAT(__defer_obj_, __LINE__) = [&]() -> void

using s8  = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

static_assert(sizeof(s8)  == 1);
static_assert(sizeof(s16) == 2);
static_assert(sizeof(s32) == 4);
static_assert(sizeof(s64) == 8);
static_assert(sizeof(u8)  == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

using ssize = ptrdiff_t;
using usize = size_t;

static_assert(sizeof(ssize) == sizeof(void*));
static_assert(sizeof(usize) == sizeof(void*));

using f32 = float;
using f64 = double;

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

template <usize N>
constexpr usize countof(auto (&)[N]) { return N; }

constexpr void unused(auto &&...) {}

template <typename T> struct RemoveReference        { using Type = T; };
template <typename T> struct RemoveReference<T &>   { using Type = T; };
template <typename T> struct RemoveReference<T &&>  { using Type = T; };
template <typename T> using RemoveReferenceT = typename RemoveReference<T>::Type;

#define MOV(...)    ((RemoveReferenceT<decltype(__VA_ARGS__)> &&)(__VA_ARGS__))
#define FWD(...)    ((decltype(__VA_ARGS__) &&)(__VA_ARGS__))

constexpr auto Min(auto a, auto b)             { return a < b ? a : b; }
constexpr auto Max(auto a, auto b)             { return a > b ? a : b; }
constexpr auto Clamp(auto v, auto lo, auto hi) { return Max(lo, Min(v, hi)); }

template <typename ...Args>
constexpr void ImplError_(const char* file, u32 line, fmt::format_string<Args...> fmt, Args &&...args)
{
    fmt::print(stderr, fg(fmt::color::red), "{}:{}: Error: ", file, line);
    fmt::println(stderr, fmt, FWD(args)...);
}

template <typename ...Args>
constexpr void ImplWarning_(const char* file, u32 line, fmt::format_string<Args...> fmt, Args &&...args)
{
    fmt::print(stderr, fg(fmt::color::yellow), "{}:{}: Warning: ", file, line);
    fmt::println(stderr, fmt, FWD(args)...);
}

template <typename ...Args>
constexpr void ImplInfo_(const char* file, u32 line, fmt::format_string<Args...> fmt, Args &&...args)
{
    fmt::print(fg(fmt::color::blue), "{}:{}: Info: ", file, line);
    fmt::println(fmt, FWD(args)...);
}

#define ERROR(...)      ImplError_(__FILE__, __LINE__, __VA_ARGS__)
#define WARNING(...)    ImplWarning_(__FILE__, __LINE__, __VA_ARGS__)
#define INFO(...)       ImplInfo_(__FILE__, __LINE__, __VA_ARGS__)

#if defined(_MSC_VER)
    #define DEBUGBREAK()    __debugbreak()
#elif defined(__clang__)
    #define DEBUGBREAK()    __builtin_debugtrap()
#else
    #define DEBUGBREAK()    __builtin_trap()
#endif

#if !defined(ENABLE_ASSERTS)
    #if PRODUCTION_BUILD
        #define ENABLE_ASSERTS  0
    #else
        #define ENABLE_ASSERTS  1
    #endif
#endif

#if ENABLE_ASSERTS
    #define ASSERT(cond, ...)                                                                               \
        do {                                                                                                \
            if (!(cond)) [[unlikely]] {                                                                     \
                fmt::println(stderr, "{}:{}: ASSERTION FAILED: {}", __FILE__, __LINE__, PP_TOSTRING(cond)); \
                __VA_OPT__( fmt::println(stderr, "> Message: " __VA_ARGS__); )                              \
                DEBUGBREAK();                                                                               \
            }                                                                                               \
        } while (0)
#else // ENABLE_ASSERTS
    #define ASSERT(cond, ...)
#endif // ENABLE_ASSERTS

struct NonCopiable
{
    NonCopiable() = default;
    NonCopiable(const NonCopiable&) = delete;
    NonCopiable& operator =(const NonCopiable&) = delete;
};

