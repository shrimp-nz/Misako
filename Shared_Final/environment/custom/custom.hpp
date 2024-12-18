#pragma once
#include <Windows.h>
#include <cstdint>
#include <filesystem>
#include <dllmain.hpp>

namespace environment
{
	std::int32_t get_genv(misako_State* state);
	std::int32_t get_renv(misako_State* state);
	std::int32_t get_tenv(misako_State* state);
	std::int32_t get_reg(misako_State* state);
	std::int32_t get_gc(misako_State* state);

	std::int32_t get_thread_identity(misako_State* state);
	std::int32_t set_thread_identity(misako_State* state);

	std::int32_t get_fast_flag(misako_State* state);
	std::int32_t set_fast_flag(misako_State* state);

	std::int32_t get_clipboard(misako_State* state);
	std::int32_t set_clipboard(misako_State* state);

	std::int32_t get_raw_metatable(misako_State* state);
	std::int32_t set_raw_metatable(misako_State* state);

	std::int32_t get_namecall_method(misako_State* state);
	std::int32_t set_namecall_method(misako_State* state);

	std::int32_t get_networked_datamodel(misako_State* state);
	std::int32_t set_networked_datamodel(misako_State* state);

	std::int32_t get_fps_cap(misako_State* state);
	std::int32_t set_fps_cap(misako_State* state);

	std::int32_t get_all_properties(misako_State* state);
	std::int32_t get_hidden_properties(misako_State* state);

	std::int32_t is_scriptable(misako_State* state);
	std::int32_t set_scriptable(misako_State* state);

	std::int32_t set_readonly(misako_State* state);
	std::int32_t is_readonly(misako_State* state);

	std::int32_t is_cclosure(misako_State* state);
	std::int32_t is_lclosure(misako_State* state);

	std::int32_t new_cclosure_handler(misako_State* state);
	std::int32_t new_cclosure(misako_State* state);

	std::int32_t fire_clickdetector(misako_State* state);
	std::int32_t fire_touchtransmitter(misako_State* state);
	std::int32_t fire_proximityprompt(misako_State* state);

	std::int32_t http_request(misako_State* state);
	std::int32_t get_hidden_gui(misako_State* state);
	std::int32_t queue_on_teleport(misako_State* state);
	std::int32_t messagebox_async(misako_State* state);
	std::int32_t hook_function(misako_State* state);
	std::int32_t load_bytecode(misako_State* state);
	std::int32_t check_caller(misako_State* state);
	std::int32_t identify_executor(misako_State* state);

	std::int32_t clone_ref(misako_State* state);
	std::int32_t clone_function(misako_State* state);
	std::int32_t compare_instances(misako_State* state);

	std::int32_t is_our_closure(misako_State* state);
	std::int32_t is_rbx_active(misako_State* state);
	std::int32_t is_readonly(misako_State* state);

	std::int32_t get_connections(misako_State* state);
	std::int32_t get_roblox_path(misako_State* state);
	std::int32_t get_script_bytecode(misako_State* state);
	std::int32_t get_script_closure(misako_State* state);

	std::int32_t decompile_luau_bytecode(misako_State* state);
	std::int32_t dissassemble_luau_bytecode(misako_State* state);

	std::int32_t lz4_compress(misako_State* state);
	std::int32_t lz4_decompress(misako_State* state);

	void setup_custom_environment(misako_State* state);
}

namespace image_library
{
	/*

	returning table:
	{

		height = ??, <int>
		width = ??,	<int>

		img_data = {

			[y_axis/height position]<int> = {

				[x_axis/width position]<int> = {R,G,B <int, int, int>}

			}

		}

	}

	*/
	std::int32_t open(misako_State* state);
}

namespace unsafe_environment
{
	std::int32_t enable(misako_State* state);
	std::int32_t activate(misako_State* state);

	namespace request
	{
		/* Url<string>, (0: exact url, 1: containing url, 2: all requests)<int>, Edit<string>. */
		std::int32_t add_edit_return(misako_State* state);
		/* Url<string> */
		std::int32_t remove_edit_return(misako_State* state);
	}
}