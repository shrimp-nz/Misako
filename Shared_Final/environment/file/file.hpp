#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

#include <dllmain.hpp>

namespace io_environment
{
	std::int32_t read_file(misako_State* state);
	std::int32_t write_file(misako_State* state);
	std::int32_t append_file(misako_State* state);
	std::int32_t load_file(misako_State* state);
	std::int32_t list_file(misako_State* state);

	std::int32_t make_folder(misako_State* state);

	std::int32_t is_file(misako_State* state);
	std::int32_t is_folder(misako_State* state);

	std::int32_t delete_file(misako_State* state);
	std::int32_t delete_folder(misako_State* state);

	// UNSAFE, MAKE SURE TO HIDE.
	std::int32_t unsafe_makefolder(misako_State* state);
	std::int32_t unsafe_clonefile(misako_State* state);

	void setup_io_environment(misako_State* state);
}