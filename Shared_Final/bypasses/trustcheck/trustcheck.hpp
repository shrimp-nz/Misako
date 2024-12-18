#pragma once
#include <Windows.h>
#include <cstdint>

extern std::uintptr_t ar_trustcheck_hook;

namespace trustcheck
{
    void bypass();
}