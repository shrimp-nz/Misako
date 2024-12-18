#pragma once
constexpr auto module_name = L"dbghelp.dll";
constexpr auto exploit_name = "Misako";

// set to true if you want owlhub to break :nerd:
// #define use_xorstr false // MOVED TO PREPROCESSOR DEFINITIONS

// FOR AUTOLAUNCH, MAKE A REGEDIT WRAP FOR THIS -rexi

constexpr auto use_task_spawn = false;
constexpr auto auto_launch = false;

#include <dependencies/luau/VM/src/lualib.h>