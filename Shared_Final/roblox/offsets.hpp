#pragma once
#include <Windows.h>

#include <cstdint>
#include <cmath>
#include <functional>
#include <corecrt_math.h>

#include <dependencies/xorstring/xorstring.hpp>
#include <bypasses/memcheck/utilities.hpp>

constexpr bool compare(const std::uint8_t* pData, const std::uint8_t* bMask, const char* szMask)
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return 0;

    return (*szMask) == NULL;
}

__inline std::uintptr_t scan_mask(const char* AoB, const char* Mask)
{
    for (auto i = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr)); i <= 0xF000000; ++i)
    {
        if (compare(reinterpret_cast<std::uint8_t*>(i), (std::uint8_t*)(AoB), Mask))
            return i;
    }
    return 0;
}

/* offsets */
const auto datamodel_networked = 1376; // "Empty URL", != 3

const auto extraspace_identity = 72;
const auto extraspace_script = 24;

const auto taskscheduler_jobname = 16;
const auto taskscheduler_jobstart = 308;
const auto taskscheduler_jobend = 312;
const auto taskscheduler_datamodel = 40;
const auto taskscheduler_scriptcontext = 304;

const auto taskscheduler_delayed = 280;

const auto rbxscriptsignal_scriptconnection = 28;
const auto rbxscriptsignal_nextsignal = 16;
const auto rbxscriptsignal_state = 20;

template < typename T = std::uintptr_t >
T get_thread_context(const std::uintptr_t state)
{
    const auto identity = *reinterpret_cast<std::uintptr_t*>(state + extraspace_identity);
    return T(*reinterpret_cast<std::uintptr_t*>(identity + extraspace_script));
}

template < typename T = std::uintptr_t >
T set_thread_context(const std::uintptr_t state, std::int32_t idx)
{
    const auto identity = *reinterpret_cast<std::uintptr_t*>(state + extraspace_identity);
    return T(*reinterpret_cast<std::uintptr_t*>(identity + extraspace_script) = idx);
}

/* sigs */
const auto clickdetector_address = mem_utils::unbase(0x0E85B10); // not updated
const auto touchinterest_address = mem_utils::unbase(0x9F8D40);
const auto proximityprompt_address = mem_utils::unbase(0xfa1590); // not updated

const auto trustcheck_patch = scan_mask("\x75\x4D\x83\x7E\x14\x10", "xxxxxx");

const auto taskscheduler_signature = scan_mask("\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x08\xE8\x00\x00\x00\x00\x8D\x0C\x24", "xxxxxxxxxx????xxx");
const auto taskscheduler_address = (taskscheduler_signature + 14) + *reinterpret_cast<std::uint32_t*>(taskscheduler_signature + 10);

const auto flog_getvalue_address = mem_utils::unbase(0x01DC0800);
const auto flog_setvalue_address = mem_utils::unbase(0x01DC0180);

const auto luau_get_state_address = mem_utils::unbase(0x086F580);

const auto luau_load_address = mem_utils::unbase(0x018DE190);
const auto scriptcontext_resume_address = mem_utils::unbase(0x8935E0);

const auto ret_hook_address = mem_utils::unbase(0x4173C1C);
const auto ret_push_address = mem_utils::unbase(0x06FA4D2);

const auto properties_array_addr = mem_utils::unbase(0x3B44B00);
const auto get_property_addr = mem_utils::unbase(0x982840);

const auto r_taskscheduler = reinterpret_cast<std::uintptr_t(*)()>(taskscheduler_address);
const auto r_firetouchinterest = reinterpret_cast<void(__thiscall*)(std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t)>(touchinterest_address);

const auto flog_getvalue = reinterpret_cast<bool(__fastcall*)(const std::string&, std::string&, bool)>(flog_getvalue_address);
const auto flog_setvalue = reinterpret_cast<bool(__fastcall*)(const std::string&, const std::string&, std::int32_t, bool)>(flog_setvalue_address);

const auto get_property = reinterpret_cast<std::uintptr_t(__thiscall*)(std::uintptr_t, std::uintptr_t)>(get_property_addr);
const auto properties_array = reinterpret_cast<std::uintptr_t*>(properties_array_addr);

__forceinline std::int32_t r_set_lua_state(const std::uintptr_t state)
{
    std::int32_t ptr = 0;
    const auto lua_state = reinterpret_cast<std::uintptr_t(__thiscall*)(std::uintptr_t, std::int32_t*)>(luau_get_state_address)
        (state, &ptr);

    return lua_state;
}

__inline std::int32_t r_luau_load(const std::uintptr_t state, const char* chunk, const char* bytecode, const std::size_t size, std::int32_t env)
{
    reinterpret_cast<std::uintptr_t(__fastcall*)(const std::uintptr_t, const char*, const char*, const std::size_t, std::int32_t)>
        (luau_load_address)(state, chunk, bytecode, size, env);
    __asm add esp, 12;
}