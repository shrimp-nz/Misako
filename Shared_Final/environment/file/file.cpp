#include "file.hpp"

#include <Luau/Compiler.h>
#include <dependencies/luau/VM/src/lfunc.h>

#include <utilities/tools/tools.hpp>
#include <utilities/console_debug/console_dbg.hpp>

#include <filesystem>
#include <bypasses/callcheck/callcheck.hpp>
#include <roblox/offsets.hpp>

std::int32_t io_environment::read_file(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size;
	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	if (!std::filesystem::exists(wpath.c_str()))
		luaL_error(state, "File does not exist in the workspace.");

	std::ifstream binarystream(wpath, std::ios_base::binary);
	std::string content((std::istreambuf_iterator<char>(binarystream)), std::istreambuf_iterator<char>());

	lua_pushlstring(state, content.c_str(), content.size());
	return 1;
}

std::int32_t io_environment::write_file(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);
	luaL_checktype(state, 2, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	std::size_t content_size;

	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto content = luaL_checklstring(state, 2, &content_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	tools::io_addons::write_file(wpath, content_size, content);
	return 0;
}

std::int32_t io_environment::append_file(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);
	luaL_checktype(state, 2, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	std::size_t content_size;

	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto content = luaL_checklstring(state, 2, &content_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	tools::io_addons::append_file(wpath, content_size, content);
	return 0;
}

std::int32_t io_environment::load_file(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it

	const auto name = luaL_checklstring(state, 1, &name_size);

	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	if (!std::filesystem::exists(wpath.c_str()))
		luaL_error(state, "File does not exist in the workspace.");

	std::ifstream binarystream(wpath, std::ios_base::binary);
	std::string content((std::istreambuf_iterator<char>(binarystream)), std::istreambuf_iterator<char>());

	const auto compiled = MisakoRVM::compile(content);

	if (r_luau_load(reinterpret_cast<const std::uintptr_t>(state), tools::io_addons::random_string(5).c_str(), compiled.c_str(), compiled.size(), 0))
	{
		lua_pushnil(state);
		lua_insert(state, -2);
		return 2;
	}


	return 1;
}

std::int32_t io_environment::list_file(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("/").append(std::string(name, name_size));
	const auto wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	lua_createtable(state, 0, 0);
	auto count = 0;
	for (auto& file_iter : std::filesystem::directory_iterator(wpath))
	{
		lua_pushnumber(state, ++count);
		lua_pushstring(state, file_iter.path().string().substr(tools::io_addons::get_dll_path("\\workspace").length() + 1).c_str());
		lua_settable(state, -3);
	}

	return 1;
}

std::int32_t io_environment::make_folder(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	std::filesystem::create_directories(wpath);
	return 0;
}

std::int32_t io_environment::is_file(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	lua_pushboolean(state, std::filesystem::is_regular_file(wpath));
	return 1;
}

std::int32_t io_environment::is_folder(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	lua_pushboolean(state, std::filesystem::is_directory(wpath));
	return 1;
}

std::int32_t io_environment::delete_file(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	if (!std::filesystem::remove(wpath))
		luaL_error(state, "File does not exist in the workspace.");

	return 0;
}

std::int32_t io_environment::delete_folder(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	const auto name = luaL_checklstring(state, 1, &name_size);
	const auto path = tools::io_addons::get_dll_path(xorstr("\\workspace")).append("\\" + std::string(name, name_size));
	const auto& wpath = std::wstring(path.begin(), path.end());

	if (wpath.find(xorstr(L"..")) != std::string::npos)
		luaL_error(state, "File attempted to escape from workspace.");

	if (!std::filesystem::remove_all(wpath))
		luaL_error(state, "Folder does not exist.");

	return 0;
}

std::int32_t io_environment::unsafe_makefolder(misako_State* state)
{
	std::size_t name_size; // better to do with sizes, also fixes getcustomasset if you use writefile with it
	const auto name = luaL_checklstring(state, 1, &name_size);

	std::size_t dir_size;
	const auto dir = luaL_checklstring(state, 2, &dir_size);

	std::string path_t = std::string(dir, dir_size);
	std::string append_t = path_t.append("\\" + std::string(name, name_size));
	std::wstring write_t = std::wstring(append_t.begin(), append_t.end());

	if (!std::filesystem::create_directories(write_t.c_str()))
		luaL_error(state, "Failed to create directory.");

	return 0;
}

std::int32_t io_environment::unsafe_clonefile(misako_State* state)
{
	std::string from = lua_tostring(state, 1);
	std::string to = lua_tostring(state, 2);

	const auto dll_path = tools::io_addons::get_dll_path("\\workspace\\").append(from);
	const auto dll_wpath = std::wstring(dll_path.begin(), dll_path.end());
	const auto to_wpath = std::wstring(to.begin(), to.end());

	CopyFileW(dll_wpath.c_str(), to_wpath.c_str(), FALSE);
	return 0;
}

static const luaL_Reg file_environment_functions[] = {
	{"readfile", io_environment::read_file},
	{"writefile", io_environment::write_file},
	{"appendfile", io_environment::append_file},
	{"loadfile", io_environment::load_file},
	{"listfiles", io_environment::list_file},
	{"makefolder", io_environment::make_folder},

	{"isfile", io_environment::is_file},
	{"isfolder", io_environment::is_folder},

	{"delfile", io_environment::delete_file},
	{"delfolder", io_environment::delete_folder},

	{"unsafe_makefolder", io_environment::unsafe_makefolder},
	{"unsafe_clonefile", io_environment::unsafe_clonefile},

	{NULL, NULL}
};

void io_environment::setup_io_environment(misako_State* state)
{
	auto to_register = file_environment_functions;

	for (; to_register->name; to_register++)
		misako_register(state, to_register->func, to_register->name);
}