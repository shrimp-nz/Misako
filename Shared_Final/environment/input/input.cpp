#include "input.hpp"

#include <Luau/Compiler.h>
#include <dependencies/luau/VM/src/lfunc.h>

#include <utilities/tools/tools.hpp>
#include <utilities/console_debug/console_dbg.hpp>

#include <bypasses/callcheck/callcheck.hpp>

#include <environment/custom/custom.hpp>

std::int32_t input_environment::mouse1_click(misako_State* state)
{
	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse1_press(misako_State* state)
{
	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse1_release(misako_State* state)
{
	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse2_click(misako_State* state)
{
	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse2_press(misako_State* state)
{
	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse2_release(misako_State* state)
{
	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse_moverel(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TNUMBER);
	luaL_checktype(state, 2, LUA_TNUMBER);

	const auto x = lua_tonumber(state, -2);
	const auto y = lua_tonumber(state, -1);

	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse_moveabs(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TNUMBER);
	luaL_checktype(state, 2, LUA_TNUMBER);

	auto x = lua_tonumber(state, -2);
	auto y = lua_tonumber(state, -1);

	auto width = GetSystemMetrics(SM_CXSCREEN) - 1;
	auto height = GetSystemMetrics(SM_CYSCREEN) - 1;

	RECT CRect;
	GetClientRect(GetForegroundWindow(), &CRect);

	POINT Point{CRect.left, CRect.top};
	ClientToScreen(GetForegroundWindow(), &Point);

	x = (x + static_cast<const std::uintptr_t>(Point.x)) * (65535 / width);
	y = (y + static_cast<const std::uintptr_t>(Point.y)) * (65535 / height);

	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y, 0, 0);
	return 0;
}

std::int32_t input_environment::mouse_scroll(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TNUMBER);
	const auto scrollAmount = lua_tonumber(state, 1);

	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrollAmount, 0);
	return 0;
}

std::int32_t input_environment::key_press(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TNUMBER);
	const auto key = lua_tonumber(state, 1);

	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		keybd_event(0, static_cast<std::uint8_t>(MapVirtualKeyA(key, MAPVK_VK_TO_VSC)), KEYEVENTF_SCANCODE, 0);
	return 0;
}

std::int32_t input_environment::key_tap(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TNUMBER);
	const auto key = lua_tonumber(state, 1);

	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		keybd_event(0, static_cast<std::uint8_t>(MapVirtualKeyA(key, MAPVK_VK_TO_VSC)), KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP, 0);
	return 0;
}

std::int32_t input_environment::key_release(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TNUMBER);
	const auto key = lua_tonumber(state, 1);

	if (GetForegroundWindow() == FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str()))
		keybd_event(0, static_cast<std::uint8_t>(MapVirtualKeyA(key, MAPVK_VK_TO_VSC)), KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP, 0);
	return 0;
}

std::int32_t input_environment::left_click(misako_State* state)
{
	const auto idx = lua_tointeger(state, 2);

	switch (idx)
	{
	case 0: {
		input_environment::mouse1_click(state);
		break;
	}
	case 1: {
		input_environment::mouse1_press(state);
		break;
	}
	case 2: {
		input_environment::mouse1_release(state);
		break;
	}
	default: {
		luaL_error(state, "incorrect index <input.leftclick>");
		break;
	}
	}

	return 1;
}

std::int32_t input_environment::right_click(misako_State* state)
{
	const auto idx = lua_tointeger(state, 2);

	switch (idx)
	{
	case 0: {
		input_environment::mouse2_click(state);
		break;
	}
	case 1: {
		input_environment::mouse2_press(state);
		break;
	}
	case 2: {
		input_environment::mouse2_release(state);
		break;
	}
	default: {
		luaL_error(state, "incorrect index <input.rightclick>");
		break;
	}
	}

	return 1;
}

static const luaL_Reg input_environment_functions[] = {
	{"mouse1click", input_environment::mouse1_click},
	{"mouse1press", input_environment::mouse1_press},
	{"mouse1release", input_environment::mouse1_release},

	{"mouse2click", input_environment::mouse2_click},
	{"mouse2press", input_environment::mouse2_press},
	{"mouse2release", input_environment::mouse2_release},

	{"mousemoverel", input_environment::mouse_moverel},
	{"mousemoveabs", input_environment::mouse_moveabs},
	{"mousescroll", input_environment::mouse_scroll},

	{"keypress", input_environment::key_press},
	{"keytap", input_environment::key_tap},
	{"keyrelease", input_environment::key_release},

	{NULL, NULL}
};

void input_environment::setup_input_environment(misako_State* state)
{
	auto to_register = input_environment_functions;

	for (; to_register->name; to_register++)
		misako_register(state, to_register->func, to_register->name);

	lua_newtable(state);
	{
		misako_register_table(state, input_environment::mouse_moverel, "MoveMouse");

		misako_register_table(state, input_environment::left_click, "LeftClick");
		misako_register_table(state, input_environment::right_click, "RightClick");
	}
	lua_setglobal(state, "Input");
}
