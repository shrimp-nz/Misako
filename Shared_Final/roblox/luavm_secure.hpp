#pragma once
#include <cstdint>
#include <emmintrin.h>
#include <Windows.h>
#include <iostream>

#define roblox(x) (x - 0x400000 + reinterpret_cast<std::uintptr_t>(GetModuleHandleA(0)))
// reason why I made another rebasing macro is because on injection if i use the same function, it gets delayed and i dont know why

#define LUAVM_SHUFFLE_COMMA ,
#define LUAVM_SHUFFLE_SPLIT ;

#define LUAVM_SHUFFLE2(sep, a0, a1) a0 sep a1
#define LUAVM_SHUFFLE3(sep, a0, a1, a2) a1 sep a0 sep a2
#define LUAVM_SHUFFLE4(sep, a0, a1, a2, a3) a1 sep a2 sep a0 sep a3
#define LUAVM_SHUFFLE5(sep, a0, a1, a2, a3, a4) a2 sep a4 sep a3 sep a0 sep a1
#define LUAVM_SHUFFLE6(sep, a0, a1, a2, a3, a4, a5) a1 sep a5 sep a0 sep a4 sep a3 sep a2
#define LUAVM_SHUFFLE7(sep, a0, a1, a2, a3, a4, a5, a6) a0 sep a1 sep a6 sep a5 sep a3 sep a4 sep a2
#define LUAVM_SHUFFLE8(sep, a0, a1, a2, a3, a4, a5, a6, a7) a4 sep a0 sep a7 sep a2 sep a3 sep a1 sep a6 sep a5

/* addresses */
#define xor_addr roblox(0x03B893B0)
#define dummynode_addr roblox(0x02629680)
#define nilobject_addr roblox(0x02629078)

/* encryption macros */
#define LuaVMValue1_deobf_symbol -
#define LuaVMValue1_deobf_order 1
#define LuaVMValue1_obf_symbol -
#define LuaVMValue1_obf_order 1

#define LuaVMValue2_deobf_symbol -
#define LuaVMValue2_deobf_order 0
#define LuaVMValue2_obf_symbol +
#define LuaVMValue2_obf_order 1

#define LuaVMValue3_deobf_symbol ^
#define LuaVMValue3_deobf_order 1
#define LuaVMValue3_obf_symbol ^
#define LuaVMValue3_obf_order 1

#define LuaVMValue4_deobf_symbol +
#define LuaVMValue4_deobf_order 1
#define LuaVMValue4_obf_symbol -
#define LuaVMValue4_obf_order 0

/* quick macros */
#define vmv_lua_state_global  LuaVMValue1
#define vmv_lua_state_stacksize  LuaVMValue1

#define vmv_global_state_tmname  LuaVMValue1
#define vmv_global_state_ttname  LuaVMValue1

#define vmv_tstring_hash  LuaVMValue3
#define vmv_tstring_len  LuaVMValue4

#define vmv_table_array  LuaVMValue1
#define vmv_table_node  LuaVMValue1
#define vmv_table_metatable  LuaVMValue1

#define vmv_closure_c_debugname  LuaVMValue3
#define vmv_closure_c_f  LuaVMValue1
#define vmv_closure_c_cont  LuaVMValue2
#define vmv_closure_l_p  LuaVMValue1

#define vmv_udata_metatable  LuaVMValue4

#define vmv_proto_gen  LuaVMValue1
#define vmv_proto_source  LuaVMValue1
#define vmv_proto_k  LuaVMValue1

#define vmv_proto_debugname  LuaVMValue2
#define vmv_proto_debuginsn  LuaVMValue4

class LuaSecureDouble
{
private:
    double storage;

public:
    operator const double() const
    {
        const auto luaXorMask = *reinterpret_cast<double*>(xor_addr);

        __m128d xmmKey = _mm_load_pd(&luaXorMask);
        __m128d xmmData = _mm_load_sd(&storage);
        __m128d xmmResult = _mm_xor_pd(xmmData, xmmKey);
        return _mm_cvtsd_f64(xmmResult);
    }

    void operator=(const double& value)
    {
        const auto luaXorMask = *reinterpret_cast<double*>(xor_addr);

        __m128d xmmKey = _mm_load_pd(&luaXorMask);
        __m128d xmmData = _mm_load_sd(&value);
        __m128d xmmResult = _mm_xor_pd(xmmData, xmmKey);
        storage = _mm_cvtsd_f64(xmmResult);
    }
};

/* LS->GS */
template <typename T> class LuaVMValue1
{
public:
    /*
    This = addr
    storage/value = data (PTR ENC)
    (1 = (addr ^ data) :  0 = (data ^ addr))
    */
    operator const T() const /* Deobfuscate */
    {

#if LuaVMValue1_deobf_order == 1
        return (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue1_deobf_symbol(uintptr_t)storage);
#else
        return (T)((uintptr_t)storage LuaVMValue1_deobf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

    void operator=(const T& value) /* Obfuscate */
    {

#if LuaVMValue1_obf_order == 1
        storage = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue1_obf_symbol(uintptr_t)value);
#else
        storage = (T)((uintptr_t)value LuaVMValue1_obf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

//    void operator=(const LuaVMValue1& value)
//    {
//
//#if LuaVMValue1_deobf_order == 1
//        value = (T)((uintptr_t)(value.operator const T()) LuaVMValue1_deobf_symbol(uintptr_t)(this));
//#else
//        value = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue1_deobf_symbol(uintptr_t)(value.operator const T()));
//#endif
//
//    }

    const T operator->() const
    {
        return operator const T();
    }

    const T get() const
    {
        return operator const T();
    }

private:
    T storage;
};

/* Tstring->Hash */
template <typename T> class LuaVMValue2
{
public:
    /*
    This = addr
    storage/value = data
    (1 = (addr ^ data) :  0 = (data ^ addr))
    */
    operator const T() const /* Deobfuscate */
    {

#if LuaVMValue2_deobf_order == 1
        return (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue2_deobf_symbol(uintptr_t)storage);
#else
        return (T)((uintptr_t)storage LuaVMValue2_deobf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

    void operator=(const T& value) /* Obfuscate */
    {

#if LuaVMValue2_obf_order == 1
        storage = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue2_obf_symbol(uintptr_t)value);
#else
        storage = (T)((uintptr_t)value LuaVMValue2_obf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

    void operator=(const LuaVMValue2& value)
    {
#if LuaVMValue2_obf_order == 1
        value = (T)((uintptr_t)(value.operator const T()) LuaVMValue2_obf_symbol(uintptr_t)(this));
#else
        value = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue2_obf_symbol(uintptr_t)(value.operator const T()));
#endif
    }

    const T operator->() const
    {
        return operator const T();
    }

    const T get() const
    {
        return operator const T();
    }

private:
    T storage;
};

/* Proto->K  */
template <typename T> class LuaVMValue3
{
public:
    /*
    This = addr
    storage/value = data
    (1 = (addr ^ data) :  0 = (data ^ addr))
    */
    operator const T() const /* Deobfuscate */
    {

#if LuaVMValue3_deobf_order == 1
        return (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue3_deobf_symbol(uintptr_t)storage);
#else
        return (T)((uintptr_t)storage LuaVMValue3_deobf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

    void operator=(const T& value) /* Obfuscate */
    {

#if LuaVMValue3_obf_order == 1
        storage = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue3_obf_symbol(uintptr_t)value);
#else
        storage = (T)((uintptr_t)value LuaVMValue3_obf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

    void operator=(const LuaVMValue3& value)
    {
#if LuaVMValue3_obf_order == 1
        value = (T)((uintptr_t)(value.operator const T()) LuaVMValue3_obf_symbol(uintptr_t)(this));
#else
        value = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue3_obf_symbol(uintptr_t)(value.operator const T()));
#endif
    }

    const T operator->() const
    {
        return operator const T();
    }

    const T get() const
    {
        return operator const T();
    }

private:
    T storage;
};

/* Proto->Source  */
template <typename T> class LuaVMValue4
{
public:
    /*
    This = addr
    storage/value = data
    (1 = (addr ^ data) :  0 = (data ^ addr))
    */
    operator const T() const /* Deobfuscate */
    {
#if LuaVMValue4_deobf_order == 1
        return (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue4_deobf_symbol(std::uintptr_t)storage);
#else
        return (T)((uintptr_t)storage LuaVMValue4_deobf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

    void operator=(const T& value) /* Obfuscate */
    {

#if LuaVMValue4_obf_order == 1
        storage = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue4_obf_symbol(uintptr_t)value);
#else
        storage = (T)((uintptr_t)value LuaVMValue4_obf_symbol reinterpret_cast<uintptr_t>(this));
#endif

    }

    void operator=(const LuaVMValue4& value)
    {
#if LuaVMValue4_obf_order == 1
        value = (T)((uintptr_t)(value.operator const T()) LuaVMValue4_obf_symbol(uintptr_t)(this));
#else
        value = (T)(reinterpret_cast<uintptr_t>(this) LuaVMValue4_obf_symbol(uintptr_t)(value.operator const T()));
#endif
    }

    const T operator->() const
    {
        return operator const T();
    }

    const T get() const
    {
        return operator const T();
    }

private:
    T storage;
};