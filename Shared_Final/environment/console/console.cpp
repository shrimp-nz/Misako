#include "console.hpp"

#include <Luau/Compiler.h>
#include <dependencies/luau/VM/src/lfunc.h>

#include <utilities/tools/tools.hpp>
#include <utilities/console_debug/console_dbg.hpp>

#include <filesystem>
#include <bypasses/callcheck/callcheck.hpp>
#include <dependencies/imgui/imgui.h>

std::int32_t console_environment::show(misako_State* state)
{
	console.show();

	return 0;
}

std::int32_t console_environment::hide(misako_State* state)
{
	console.hide();

	return 0;
}

std::int32_t console_environment::name(misako_State* state)
{
	//luaL_checktype(state, 1, LUA_TSTRING);

	const auto name = lua_tolstring(state, 1, nullptr);

	SetConsoleTitleA(name);
	return 0;
}

std::int32_t console_environment::clear(misako_State* state)
{
	system(xorstr("cls"));
	return 0;
}

std::int32_t console_environment::info(misako_State* state)
{
	//luaL_checktype(state, 1, LUA_TSTRING);

	console.show();

	const auto content = lua_tolstring(state, 1, nullptr);
	console.write< log_type::info >({xorstr("roblox"), xorstr("info")}, content);
	return 0;
}

std::int32_t console_environment::print(misako_State* state)
{
	//luaL_checktype(state, 1, LUA_TSTRING);

	console.show();

	const auto content = lua_tolstring(state, 1, nullptr);
	console.write< log_type::info >({xorstr("roblox"), xorstr("print")}, content);
	return 0;
}

std::int32_t console_environment::warn(misako_State* state)
{
	//luaL_checktype(state, 1, LUA_TSTRING);

	console.show();

	const auto content = lua_tolstring(state, 1, nullptr);
	console.write< log_type::warn >({xorstr("roblox"), xorstr("warn")}, content);
	return 0;
}

std::int32_t console_environment::error(misako_State* state)
{
	//luaL_checktype(state, 1, LUA_TSTRING);

	console.show();

	const auto content = lua_tolstring(state, 1, nullptr);
	console.write< log_type::error >({xorstr("roblox"), xorstr("error")}, content);
	return 0;
}

std::int32_t console_environment::input(misako_State* state)
{
	console.show();

	std::string cIN;
	std::getline(std::cin, cIN);

	lua_pushstring(state, cIN.c_str());
	return 0;
}

std::int32_t console_environment::printconsole(misako_State* state)
{
	//luaL_checktype(state, 1, LUA_TSTRING);
	//luaL_checktype(state, 2, LUA_TNUMBER);
	//luaL_checktype(state, 3, LUA_TNUMBER);
	//luaL_checktype(state, 4, LUA_TNUMBER);

	//auto avg_color = 0;

	//const auto content = luaL_checklstring(state, 1, nullptr);
	//const auto red = luaL_checknumber(state, 2);
	//const auto green = luaL_checknumber(state, 3);
	//const auto blue = luaL_checknumber(state, 4);

	//avg_color = ImGui::GetColorU32(ImVec4(red / 255, green / 255, blue / 255, 1));

	//console.write< log_type::i_wish_we_could_edit >({ xorstr("roblox"),  xorstr("error") }, content);
	return 1;
}

static const luaL_Reg console_environment_functions[] = {
	{"rconsoleshow", console_environment::show},
	{"rconsolehide", console_environment::hide},
	{"rconsolename", console_environment::name},
	{"rconsoleclear", console_environment::clear},

	{"rconsoleinfo", console_environment::info},
	{"rconsoleprint", console_environment::print},
	{"rconsolewarn", console_environment::warn},
	{"rconsoleerror", console_environment::error},
	{"rconsoleinput", console_environment::input},
	//{"printconsole", console_environment::printconsole},

	{NULL, NULL}
};

void console_environment::setup_console_environment(misako_State* state)
{
	auto to_register = console_environment_functions;

	for (; to_register->name; to_register++)
		misako_register(state, to_register->func, to_register->name);
}
