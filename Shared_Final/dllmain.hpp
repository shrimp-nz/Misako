#pragma once
#include <queue>
#include <string>

#include <dependencies/luau/VM/src/lua.h>
#include <dependencies/luau/VM/src/lobject.h>
#include <dependencies/luau/VM/src/lstate.h>
#include <dependencies/luau/VM/src/lmem.h>
#include <dependencies/luau/VM/src/lapi.h>

extern std::queue<std::string> scheduled_script;
extern std::queue<std::string> after_teleport_script;

namespace states
{
	extern misako_State* global_state;
	extern misako_State* lua_state;
}

namespace main
{
	extern bool execute(const std::string& script);
}