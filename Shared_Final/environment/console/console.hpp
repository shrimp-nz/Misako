#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

#include <dllmain.hpp>

namespace console_environment
{
	std::int32_t show(misako_State* state);
	std::int32_t hide(misako_State* state);
	std::int32_t name(misako_State* state);
	std::int32_t clear(misako_State* state);

	std::int32_t info(misako_State* state);
	std::int32_t print(misako_State* state);
	std::int32_t warn(misako_State* state);
	std::int32_t error(misako_State* state);
	std::int32_t input(misako_State* state);

	std::int32_t printconsole(misako_State* state);

	void setup_console_environment(misako_State* state);
}