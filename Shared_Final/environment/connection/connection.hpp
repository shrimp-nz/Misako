#pragma once
#include <Windows.h>
#include <cstdint>
#include <filesystem>
#include <unordered_map>

#include <dllmain.hpp>

struct connection_struct
{
	std::uintptr_t signal_idx;
	std::uintptr_t old_state;
};

extern std::unordered_map<std::uintptr_t, connection_struct> connections;

namespace connection
{
	std::int32_t index_connection(misako_State* state);

	std::int32_t enable_connection(misako_State* state);
	std::int32_t disable_connection(misako_State* state);
	std::int32_t fire_connection(misako_State* state);

	std::int32_t blank_function(misako_State* state);
}