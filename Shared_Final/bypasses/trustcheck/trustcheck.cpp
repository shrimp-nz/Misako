#include "trustcheck.hpp"
#include <dependencies/xorstring/xorstring.hpp>
#include <roblox/offsets.hpp>

void trustcheck::bypass()
{
    DWORD old_protect, new_protect;

    VirtualProtect(reinterpret_cast<void*>(trustcheck_patch), 1, PAGE_EXECUTE_READWRITE, &old_protect);
    *reinterpret_cast<std::uint8_t*>(trustcheck_patch) = 0xEB;
    VirtualProtect(reinterpret_cast<void*>(trustcheck_patch), 1, old_protect, &new_protect);
}