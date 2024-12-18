// This file is part of the MisakoRVM programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

// Compiler codegen control macros
#ifdef _MSC_VER
#define LUAU_NORETURN __declspec(noreturn)
#define LUAU_NOINLINE __declspec(noinline)
#define LUAU_FORCEINLINE __forceinline
#define LUAU_LIKELY(x) x
#define LUAU_UNLIKELY(x) x
#define LUAU_UNREACHABLE() __assume(false)
#define LUAU_DEBUGBREAK() __debugbreak()
#else
#define LUAU_NORETURN __attribute__((__noreturn__))
#define LUAU_NOINLINE __attribute__((noinline))
#define LUAU_FORCEINLINE inline __attribute__((always_inline))
#define LUAU_LIKELY(x) __builtin_expect(x, 1)
#define LUAU_UNLIKELY(x) __builtin_expect(x, 0)
#define LUAU_UNREACHABLE() __builtin_unreachable()
#define LUAU_DEBUGBREAK() __builtin_trap()
#endif

namespace MisakoRVM
{

using AssertHandler = int (*)(const char* expression, const char* file, int line, const char* function);

inline AssertHandler& assertHandler()
{
    static AssertHandler handler = nullptr;
    return handler;
}

inline int assertCallHandler(const char* expression, const char* file, int line, const char* function)
{
    if (AssertHandler handler = assertHandler())
        return handler(expression, file, line, function);

    return 1;
}

} // namespace MisakoRVM

#define NDEBUG

#if !defined(NDEBUG) || defined(LUAU_ENABLE_ASSERT)
#define LUAU_ASSERT(expr) ((void)(!!(expr) || (MisakoRVM::assertCallHandler(#expr, __FILE__, __LINE__, __FUNCTION__) && (LUAU_DEBUGBREAK(), 0))))
#define LUAU_ASSERTENABLED
#else
#define LUAU_ASSERT(expr) (void)sizeof(!!(expr))
#endif

namespace MisakoRVM
{

template<typename T>
struct FValue
{
    static FValue* list;

    T value;
    bool dynamic;
    const char* name;
    FValue* next;

    FValue(const char* name, T def, bool dynamic)
        : value(def)
        , dynamic(dynamic)
        , name(name)
        , next(list)
    {
        list = this;
    }

    operator T() const
    {
        return value;
    }
};

template<typename T>
FValue<T>* FValue<T>::list = nullptr;

} // namespace MisakoRVM

#define LUAU_FASTFLAG(flag) \
    namespace FFlag \
    { \
    extern MisakoRVM::FValue<bool> flag; \
    }
#define LUAU_FASTFLAGVARIABLE(flag, def) \
    namespace FFlag \
    { \
    MisakoRVM::FValue<bool> flag(#flag, def, false); \
    }
#define LUAU_FASTINT(flag) \
    namespace FInt \
    { \
    extern MisakoRVM::FValue<int> flag; \
    }
#define LUAU_FASTINTVARIABLE(flag, def) \
    namespace FInt \
    { \
    MisakoRVM::FValue<int> flag(#flag, def, false); \
    }

#define LUAU_DYNAMIC_FASTFLAG(flag) \
    namespace DFFlag \
    { \
    extern MisakoRVM::FValue<bool> flag; \
    }
#define LUAU_DYNAMIC_FASTFLAGVARIABLE(flag, def) \
    namespace DFFlag \
    { \
    MisakoRVM::FValue<bool> flag(#flag, def, true); \
    }
#define LUAU_DYNAMIC_FASTINT(flag) \
    namespace DFInt \
    { \
    extern MisakoRVM::FValue<int> flag; \
    }
#define LUAU_DYNAMIC_FASTINTVARIABLE(flag, def) \
    namespace DFInt \
    { \
    MisakoRVM::FValue<int> flag(#flag, def, true); \
    }
