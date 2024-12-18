#include <iostream>

#include "custom.hpp"

#include <dependencies/xorstring/xorstring.hpp>

#include <roblox/offsets.hpp>
#include <roblox/config.hpp>

#include <dependencies/task_scheduler/task_scheduler.hpp>
#include <bypasses/callcheck/callcheck.hpp>

#include <Luau/Compiler.h>

#include <utilities/tools/tools.hpp>
#include <utilities/console_debug/console_dbg.hpp>

#include <dependencies/luau/VM/src/lfunc.h>
#include <dependencies/xxhash/xxhash.hpp>
#include <zstd.h>

#include <dependencies/sha384/sha384.h>
#include <dependencies/luau_decompiler/luau_decompiler.hpp>

#include <curl/curl.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <environment/connection/connection.hpp>

#include <cpr/cpr.h>
#include <cpr/cookies.h>

#include <dependencies/luau/VM/src/lgc.h>
#include <dependencies/base64/base64.hpp>
#include <dependencies/lz4/lz4.h>
#include <utilities/tools/http_status_codes.hpp>

task_scheduler task_sched;

std::int32_t environment::get_genv(misako_State* state)
{
	if (lua_gettop(state) > 0)
		luaL_error(state, "getgenv doesn't expect any arguments.");

	lua_pushvalue(states::lua_state, LUA_GLOBALSINDEX);
	lua_xmove(states::lua_state, state, 1);
	return 1;
}

std::int32_t environment::get_renv(misako_State* state)
{
	if (lua_gettop(state) > 0)
		luaL_error(state, "getrenv doesn't expect any arguments.");

	lua_pushvalue(states::global_state, LUA_GLOBALSINDEX);
	lua_xmove(states::global_state, state, 1);
	return 1;
}

std::int32_t environment::get_tenv(misako_State* state)
{
	const auto thread = lua_tothread(state, 1);

	lua_pushvalue(thread, LUA_GLOBALSINDEX);
	lua_xmove(thread, state, 1);
	return 1;
}

std::int32_t environment::get_reg(misako_State* state)
{
	if (lua_gettop(state) > 0)
		luaL_error(state, "getgenv doesn't expect any arguments.");

	lua_pushvalue(state, LUA_REGISTRYINDEX);
	return 1;
}

std::int32_t environment::get_gc(misako_State* state) {

	bool full = false;

	if (lua_gettop(state) > 0 && lua_type(state, 1) == LUA_TBOOLEAN)
		full = lua_toboolean(state, 1);

	lua_newtable(state);

	auto cur_page = state->global->allgcopages;
	int idx{};

	while (cur_page)
	{
		char* start = 0;
		char* end = 0;
		auto block = 0;
		auto size = 0;

		luaM_getpagewalkinfo(cur_page, &start, &end, &block, &size);

		for (auto pos = start; pos != end; pos += size)
		{
			const auto gco = reinterpret_cast<GCObject*>(pos);

			if (gco->gch.tt == LUA_TFUNCTION || ((gco->gch.tt == LUA_TTABLE || gco->gch.tt == LUA_TUSERDATA) && full))
			{
				state->top->value.gc = gco;
				state->top->tt = gco->gch.tt;
				state->top++;

				lua_rawseti(state, -2, ++idx);
			}
		}

		cur_page = cur_page->gcolistnext;
	}
	return 1;
}

std::int32_t environment::get_thread_identity(misako_State* state)
{
	const auto identity = get_thread_context(reinterpret_cast<const std::uintptr_t>(state));

	lua_pushnumber(state, identity);
	return 1;
}

std::int32_t environment::set_thread_identity(misako_State* state)
{
	const auto identity = lua_tointeger(state, 1);

	set_thread_context(reinterpret_cast<const std::uintptr_t>(state), identity);
	return 0;
}

std::int32_t environment::get_fast_flag(misako_State* state)
{
	std::size_t flag_size;
	const auto flag = luaL_checklstring(state, 1, &flag_size);
	std::string to_get(flag, flag_size);

	std::string ret;

	const auto exist = flog_getvalue(to_get, ret, 1);

	if (!exist)
		luaL_error(state, "flag not found");

	lua_pushstring(state, ret.c_str());
	return 1;
}

std::int32_t environment::set_fast_flag(misako_State* state)
{
	std::size_t flag_size;
	const auto flag = luaL_checklstring(state, 1, &flag_size);
	std::string to_get(flag, flag_size);

	std::size_t value_size;
	const auto value = luaL_checklstring(state, 1, &value_size);
	std::string to_set(value, value_size);

	std::string ret;

	const auto exist = flog_getvalue(flag, ret, 1);

	if (!exist)
		luaL_error(state, "flag not found");

	flog_setvalue(to_get, to_set, 63, 0);
	return 0;
}

std::int32_t environment::get_clipboard(misako_State* state)
{
	if (lua_gettop(state) > 0)
		luaL_error(state, "getclipboard doesn't expect any arguments");

	OpenClipboard(0);

	const std::string& clip = reinterpret_cast<const char* const>(GetClipboardData(CF_TEXT));
	lua_pushstring(state, clip.c_str());

	CloseClipboard();
	return 1;
}

std::int32_t environment::set_clipboard(misako_State* state)
{
	std::size_t string_length;
	const auto string = luaL_checklstring(state, 1, &string_length);
	const auto& input = std::string(string, string_length);

	OpenClipboard(0);
	EmptyClipboard();

	const auto h_global = GlobalAlloc(GMEM_MOVEABLE, string_length + 1);
	std::memcpy(GlobalLock(h_global), input.data(), string_length + 1);

	GlobalUnlock(h_global);

	SetClipboardData(CF_TEXT, h_global);
	CloseClipboard();

	GlobalFree(h_global);
	return 0;
}

std::int32_t environment::get_raw_metatable(misako_State* state)
{
	lua_getmetatable(state, 1);
	return 1;
}

std::int32_t environment::set_raw_metatable(misako_State* state)
{
	const auto type = lua_type(state, 2);
    luaL_argcheck(state, type == LUA_TNIL || type == LUA_TTABLE, 2, "Expected table or nil");

	lua_pushvalue(state, 2);
	lua_setmetatable(state, 1);
	return 1;
}

std::int32_t environment::get_namecall_method(misako_State* state)
{
	const auto namecall = state->namecall;

	if (namecall)
	{
		state->top->value.gc = reinterpret_cast<GCObject*>(namecall);
		state->top->tt = LUA_TSTRING;
		state->top++;
	}
	else
		lua_pushnil(state);

	return 1;
}

std::int32_t environment::set_namecall_method(misako_State* state)
{
	state->namecall = tsvalue(index2addr(state, 1));
	return 1;
}

std::int32_t environment::get_networked_datamodel(misako_State* state)
{
	lua_getfield(state, -10002, "game");
	const auto instance = reinterpret_cast<const std::uintptr_t>(lua_touserdata(state, -1));
	const auto datamodel = *reinterpret_cast<const std::uintptr_t*>(instance) - 12;
	const auto networked_datamodel = reinterpret_cast<std::uintptr_t*>(datamodel + datamodel_networked);

	lua_pushnumber(state, *networked_datamodel);
	return 1;
}

std::int32_t environment::set_networked_datamodel(misako_State* state)
{
	lua_getfield(state, -10002, "game");
	const auto instance = reinterpret_cast<const std::uintptr_t>(lua_touserdata(state, -1));
	const auto datamodel = *reinterpret_cast<const std::uintptr_t*>(instance) - 12;
	const auto networked_datamodel = reinterpret_cast<std::uintptr_t*>(datamodel + datamodel_networked);

	const auto to = lua_tonumber(state, 1);
	*networked_datamodel = to;
	return 1;
}

std::int32_t environment::get_fps_cap(misako_State* state)
{
	lua_pushnumber(state, task_sched.get_fps_cap());
	return 1;
}

std::int32_t environment::set_fps_cap(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TNUMBER);
	const auto value = lua_tonumber(state, 1);
	auto frames = 1.0 / value;

	if (frames > value)
		frames = 0.01;

	task_sched.set_fps_cap(frames);
	return 1;
}

std::int32_t environment::get_all_properties(misako_State* state)
{
	if (lua_gettop(state) > 2)
		luaL_error(state, "getproperties doesn't expect any arguments above 1");

	const auto instance = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));
	const auto instance_descriptor = *reinterpret_cast<std::uintptr_t*>(instance + 0xC);

	const auto property_start = *reinterpret_cast<std::uintptr_t*>(instance_descriptor + 0x24);
	const auto property_end = *reinterpret_cast<std::uintptr_t*>(instance_descriptor + 0x28);

	if (property_start && property_end && property_start != property_end)
	{
		const auto thread_ptr = *reinterpret_cast<std::uintptr_t*>(property_start);

		lua_createtable(state, 0, 0);
		for (auto iterator = property_start + 8; iterator < property_end; iterator += 8)
		{
			const auto instance_property = *reinterpret_cast<std::uintptr_t*>(iterator + 4);
			if (instance_property == thread_ptr || instance_property == 0) continue;

			const auto property_descriptor = *reinterpret_cast<std::string**>(instance_property + 0x4);

			lua_pushstring(state, property_descriptor->c_str());

			lua_pushvalue(state, 2);
			lua_pushvalue(state, 1);
			lua_pushvalue(state, -3);

			if (lua_pcall(state, 2, 1, 0)) 
			{
				lua_pop(state, 1); // remove message
				lua_pushstring(state, "");
			}
			lua_settable(state, -3);
		}
	}
	return 1;
}

std::int32_t environment::get_hidden_properties(misako_State* state)
{
	if (lua_gettop(state) > 2)
		luaL_error(state, "gethiddenproperties doesn't expect any arguments above 1");

	const auto instance = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));
	const auto instance_descriptor = *reinterpret_cast<std::uintptr_t*>(instance + 12);

	const auto property_start = *reinterpret_cast<std::uintptr_t*>(instance_descriptor + 36);
	const auto property_end = *reinterpret_cast<std::uintptr_t*>(instance_descriptor + 40);

	if (property_start && property_end && property_start != property_end)
	{
		const auto thread_ptr = *reinterpret_cast<std::uintptr_t*>(property_start);

		lua_createtable(state, 0, 0);
		for (auto iterator = property_start + 8; iterator < property_end; iterator += 8)
		{
			const auto instance_property = *reinterpret_cast<std::uintptr_t*>(iterator + 4);
			if (instance_property == thread_ptr || instance_property == 0) continue;

			const auto property_descriptor = *reinterpret_cast<std::string**>(instance_property + 4);
			auto property_permissions = reinterpret_cast<std::int32_t*>(instance_property + 36);

			if (!(*property_permissions & (1 << 5)))
			{
				*property_permissions ^= (1 << 5);
				lua_pushstring(state, property_descriptor->c_str());

				lua_pushvalue(state, 2);
				lua_pushvalue(state, 1);
				lua_pushvalue(state, -3);

				if (lua_pcall(state, 2, 1, 0))
				{
					lua_pop(state, 1); // remove message
					lua_pushstring(state, "");
				}
				lua_settable(state, -3);
				*property_permissions ^= (1 << 5);
			}
		}
	}
	return 1;
}

std::int32_t environment::is_scriptable(misako_State* state)
{
	const auto instance = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));

	luaL_checktype(state, 2, LUA_TSTRING);

	const auto prop_descriptor = properties_array[tsvalue(index2addr(state, 2))->atom];

	if (!prop_descriptor)
		luaL_error(state, "Invalid property name");

	const auto prop = get_property(*reinterpret_cast<std::uintptr_t*>(instance + 0xC), prop_descriptor);

	if (!prop)
		luaL_error(state, "Invalid property name");

	const auto old = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(prop) + 0x20) >> 5 & 1;

	lua_pushboolean(state, old);
	return 1;
}

std::int32_t environment::set_scriptable(misako_State* state)
{
	const auto instance = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));

	luaL_checktype(state, 2, LUA_TSTRING);

	const auto prop_descriptor = properties_array[tsvalue(index2addr(state, 2))->atom];

	if (!prop_descriptor)
		luaL_error(state, "Invalid property name");

	const auto prop = get_property(*reinterpret_cast<std::uintptr_t*>(instance + 0xC), prop_descriptor);

	if (!prop)
		luaL_error(state, "Invalid property name");

	const auto old = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(prop) + 0x20) >> 5 & 1;

	if (luaL_checkboolean(state, 3))
		*reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(prop) + 0x20) |= 1 << 5;
	else
		*reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(prop) + 0x20) ^= 1 << 5;

	lua_pushboolean(state, old);
	return 1;
}

std::int32_t environment::set_readonly(misako_State* state)
{
	auto type = 0;
	auto readonly = 0;

	if (lua_gettop(state) != 2)
		luaL_error(state, "expected 2 arguments (table : tbl, number [boolean] : readonly)");

	if (lua_type(state, 1) != LUA_TTABLE)
		luaL_error(state, "argument #1 expected table");

	if ((type = lua_type(state, 2)) != LUA_TBOOLEAN && type != LUA_TNUMBER)
		luaL_error(state, "argument #2 expected number [boolean]");

	if (type == LUA_TNUMBER)
		lua_tonumber(state, 2) == 0 ? readonly = 0 : readonly = 1;
	else
		lua_toboolean(state, 2) ? readonly = 1 : readonly = 0;

	lua_setreadonly(state, 1, readonly);
	return 0;
}

std::int32_t environment::is_readonly(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TTABLE);

	const auto table = hvalue(index2addr(state, 1));
	lua_pushboolean(state, table->readonly);
	return 1;
}

std::int32_t environment::is_cclosure(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TFUNCTION);

	const auto boolean = lua_iscfunction(state, 1);
	lua_pushboolean(state, boolean);
	return 1;
}

std::int32_t environment::is_lclosure(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TFUNCTION);

	const auto boolean = lua_isLfunction(state, 1);
	lua_pushboolean(state, boolean);
	return 1;
}

std::int32_t environment::new_cclosure_handler(misako_State* state)
{
	const auto oargs = lua_gettop(state);
	const auto closure = callcheck::getnccmap()->find(clvalue(state->ci->func))->second;

	state->top->value.p = reinterpret_cast<void*>(closure);
	state->top->tt = LUA_TFUNCTION;
	state->top++;

	lua_insert(state, 1);

	const auto res = lua_pcall(state, oargs, LUA_MULTRET, 0);
	if (res && res != LUA_YIELD)
	{
		std::size_t error_len;
		const char* errmsg = luaL_checklstring(state, -1, &error_len);
		std::string error(errmsg, error_len);

		if (error == std::string(xorstr("attempt to yield across metamethod/C-call boundary")))
			return lua_yield(state, 0);

		luaL_errorL(state, luaL_checklstring(state, -1, 0));
	}

	return lua_gettop(state);
}

std::int32_t environment::new_cclosure(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TFUNCTION);

	lua_pushvalue(state, 1);
	lua_setfield(state, -10002, tools::io_addons::random_string(5).c_str());

	const auto lclosure = *reinterpret_cast<Closure**>(index2addr(state, 1));
	callcheck::lua_pushcclosure_(state, environment::new_cclosure_handler, 0);
	const auto cclosure = *reinterpret_cast<Closure**>(index2addr(state, -1));

	callcheck::nccmap[cclosure] = lclosure;

	return 1;
}

std::int32_t environment::fire_clickdetector(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TUSERDATA);
	const auto distance = static_cast<float>(luaL_optnumber(state, 2, 0));
	const auto detector = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));

	lua_getfield(state, -10002, "game");
	lua_getfield(state, -1, "GetService");
	lua_pushvalue(state, -2);
	lua_pushstring(state, "Players");
	lua_pcall(state, 2, 1, 0);
	lua_getfield(state, -1, "LocalPlayer");

	const auto player = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, -1));
	reinterpret_cast<void(__thiscall*)(std::uintptr_t, std::uintptr_t, float)>(clickdetector_address)(detector, player, distance);
	__asm { pop ecx }

	return 0;
}

std::int32_t environment::fire_touchtransmitter(misako_State* state) // https://gamer.irlgladiatorlol.xyz/images/ida_Vkk7yTA7cn.png
{
	luaL_checktype(state, 1, LUA_TUSERDATA);
	luaL_checktype(state, 2, LUA_TUSERDATA);
	luaL_checktype(state, 3, LUA_TNUMBER);

	const auto transmitter = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1)) + 0xd8);
	const auto to_touch = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 2)) + 0xd8);
	const auto world = *reinterpret_cast<std::uintptr_t*>(transmitter + 0x184);
	int touch_type = lua_tointeger(state, 3);

	reinterpret_cast<void(__thiscall*)(std::uintptr_t, std::uintptr_t, std::uintptr_t, int)>(touchinterest_address)(world, transmitter, to_touch, touch_type);
	return 0;
}

std::int32_t environment::fire_proximityprompt(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TUSERDATA);
	reinterpret_cast<int(__thiscall*)(std::uintptr_t)>(proximityprompt_address)(*reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1)));
	return 0;
}

std::int32_t environment::http_request(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TTABLE);

	lua_getfield(state, 1, "Url");

	const auto to_get = std::string(luaL_checklstring(state, -1, nullptr));

	std::string to_method;
	lua_getfield(state, 1, "Method");
	if (lua_type(state, -1) == LUA_TSTRING)
	{
		to_method = std::string(luaL_checklstring(state, -1, nullptr));

		std::transform(to_method.begin(), to_method.end(), to_method.begin(),
			[](std::uint8_t c) { return std::tolower(c); });
	}

	lua_pop(state, 1);

	cpr::Header header_property;

	lua_getfield(state, 1, "Headers");

	if (lua_type(state, -1) == LUA_TTABLE)
	{
		lua_pushnil(state);

		while (lua_next(state, -2))
		{
			auto to_header = std::string(luaL_checklstring(state, -2, nullptr));

			std::transform(to_header.begin(), to_header.end(), to_header.begin(),
				[](std::uint8_t c) { return std::tolower(c); });

			const auto header_value = std::string(luaL_checklstring(state, -1, nullptr));
			header_property.insert({to_header, header_value});

			lua_pop(state, 1);
		}
	}

	lua_pop(state, 1);

	for (const auto& headers : header_property)
	{
		auto first_header = headers.first; // intellisense doesnt like if we use this directly within std::transform

		std::transform(first_header.begin(), first_header.end(), first_header.begin(),
			[](std::uint8_t c) { return std::tolower(c); });
	}

	header_property.insert({"User-Agent", std::string("Misako ") + ("0.1")}); // useragent support!

	std::string body_property;
	lua_getfield(state, 1, "Body");
	if (lua_type(state, -1) == LUA_TSTRING)
	{
		const auto to_body = std::string(luaL_checklstring(state, -1, nullptr));
		body_property = to_body;
	}

	lua_pop(state, 1);

	cpr::Response request_method;
	cpr::Cookies cookie_property = 0;

	if (!std::strcmp(to_method.c_str(), "get"))
		request_method = cpr::Get(cpr::Url{to_get}, cookie_property, header_property);
	else if (!std::strcmp(to_method.c_str(), "head"))
		request_method = cpr::Head(cpr::Url{to_get}, cookie_property, header_property);
	else if (!std::strcmp(to_method.c_str(), "post"))
		request_method = cpr::Post(cpr::Url{to_get}, cpr::Body{body_property}, cookie_property, header_property);
	else if (!std::strcmp(to_method.c_str(), "put"))
		request_method = cpr::Put(cpr::Url{to_get}, cpr::Body{body_property}, cookie_property, header_property);
	else if (!std::strcmp(to_method.c_str(), "delete"))
		request_method = cpr::Delete(cpr::Url{to_get}, cpr::Body{body_property}, cookie_property, header_property);
	else if (!std::strcmp(to_method.c_str(), "options"))
		request_method = cpr::Options(cpr::Url{to_get}, cpr::Body{body_property}, cookie_property, header_property);
	else
		luaL_error(state, "invalid request method");

	lua_newtable(state);

	lua_pushboolean(state, (request_method.status_code >= 200 && request_method.status_code < 300));
	lua_setfield(state, -2, "Success");

	lua_pushnumber(state, request_method.status_code);
	lua_setfield(state, -2, "StatusCode");

	lua_pushstring(state, HttpStatus::code_reason(request_method.status_code).c_str());
	lua_setfield(state, -2, "StatusMessage");

	lua_newtable(state);

	for (const auto& request_header : request_method.header)
	{
		lua_pushlstring(state, request_header.first.c_str(), request_header.first.size());
		lua_pushlstring(state, request_header.second.c_str(), request_header.second.size());

		lua_settable(state, -3);
	}

	lua_setfield(state, -2, "Headers");

	lua_pushlstring(state, request_method.text.c_str(), request_method.text.size());
	lua_setfield(state, -2, "Body");
	return 1;
}

std::int32_t environment::get_hidden_gui(misako_State* state)
{
	lua_getfield(state, -10002, "game");
	lua_getfield(state, -1, "CoreGui");

	return 1;
}

std::int32_t environment::queue_on_teleport(misako_State* state)
{
	const auto to_push = luaL_checklstring(state, -1, nullptr);
	after_teleport_script.push(to_push);
	return 0;
}

std::int32_t environment::messagebox_async(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);
	luaL_checktype(state, 2, LUA_TSTRING);
	luaL_checktype(state, 3, LUA_TNUMBER);

	const auto text = luaL_checklstring(states::lua_state, 1, nullptr);
	const auto caption = luaL_checklstring(states::lua_state, 2, nullptr);
	const auto type = luaL_checkinteger(states::lua_state, 3);

	MessageBoxA(0, text, caption, type);
	return 1;
}

enum type 
{
	roblox_c_closure,
	module_c_closure,
	module_c_wrap,
	l_closure,
	not_set
};

auto get_type(Closure* cl) -> type
{
	auto cl_type = not_set;

	if (!cl->isC)
		cl_type = l_closure;
	else
	{
		if (reinterpret_cast<std::uintptr_t>(cl->c.f.get()) == ret_push_address)
		{
			if (callcheck::get(cl) == environment::new_cclosure_handler)
				cl_type = module_c_wrap;
			else
				cl_type = module_c_closure;
		}
		else
			cl_type = roblox_c_closure;
	}

	return cl_type;
}

std::int32_t environment::hook_function(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TFUNCTION);
	luaL_checktype(state, 2, LUA_TFUNCTION);

	//no im not using const auto -bogie
	const auto cl1 = clvalue(index2addr(state, 1));
	const auto cl2 = clvalue(index2addr(state, 2));
	int nups1 = cl1->nupvalues;
	int nups2 = cl2->nupvalues;

	if (get_type(cl1) == roblox_c_closure && get_type(cl2) == roblox_c_closure)
	{
		if (nups1 >= nups2)
		{
			lua_clonecfunction(state, 1);

			cl1->c.f = cl2->c.f.get();
			cl1->c.cont = cl2->c.cont.get();
			cl1->env = cl2->env;

			for (int i = 0; i < nups2; i++)
				setobj2n(state, &cl1->c.upvals[i], &cl2->c.upvals[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}

	else if (get_type(cl1) == module_c_closure && get_type(cl2) == module_c_closure)
	{
		if (nups1 >= nups2)
		{
			lua_clonecfunction(state, 1);
			callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));

			callcheck::set(cl1, callcheck::get(cl2));
			cl1->c.cont = cl2->c.cont.get();
			cl1->env = cl2->env;

			for (int i = 0; i < nups2; i++)
				setobj2n(state, &cl1->c.upvals[i], &cl2->c.upvals[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}

	else if (get_type(cl1) == module_c_wrap && get_type(cl2) == module_c_wrap)
	{
		lua_clonecfunction(state, 1);
		callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));

		callcheck::wraps::set(clvalue(index2addr(state, -1)), callcheck::wraps::get(cl1));
		callcheck::wraps::set(cl1, callcheck::wraps::get(cl2));

		cl1->env = cl2->env;
	}

	else if (get_type(cl1) == l_closure && get_type(cl2) == l_closure)
	{
		if (nups1 >= nups2)
		{
			lua_clonefunction(state, 1);

			cl1->l.p = cl2->l.p.get();
			cl1->env = cl2->env;

			for (int i = 0; i < nups2; i++)
				setobj2n(state, &cl1->l.uprefs[i], &cl2->l.uprefs[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}

	else if (get_type(cl1) == roblox_c_closure && get_type(cl2) == module_c_closure)
	{
		if (nups1 >= nups2)
		{
			lua_clonecfunction(state, 1);

			callcheck::set(cl1, callcheck::get(cl2));

			cl1->c.f = cl2->c.f.get();
			cl1->c.cont = cl2->c.cont.get();
			cl1->env = cl2->env;

			for (int i = 0; i < nups2; i++)
				setobj2n(state, &cl1->c.upvals[i], &cl2->c.upvals[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}

	else if (get_type(cl1) == roblox_c_closure && get_type(cl2) == module_c_wrap)
	{
		lua_clonecfunction(state, 1);

		callcheck::set(cl1, callcheck::get(cl2));
		callcheck::wraps::set(cl1, callcheck::wraps::get(cl2));

		cl1->c.f = cl2->c.f.get();
		cl1->c.cont = cl2->c.cont.get();
		cl1->env = cl2->env;
	}

	else if (get_type(cl1) == roblox_c_closure && get_type(cl2) == l_closure)
	{
		lua_clonecfunction(state, 1);
		lua_ref(state, 2);

		cl1->c.f = (lua_CFunction)ret_push_address;
		callcheck::set(cl1, new_cclosure_handler);
		callcheck::wraps::set(cl1, cl2);
	}

	else if (get_type(cl1) == module_c_closure && get_type(cl2) == roblox_c_closure)
	{
		if (nups1 >= nups2)
		{
			lua_clonecfunction(state, 1);
			callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));

			cl1->env = cl2->env;
			cl1->c.f = cl2->c.f.get();
			cl1->c.cont = cl2->c.cont.get();

			for (int i = 0; i < nups2; i++)
				setobj2n(state, &cl1->c.upvals[i], &cl2->c.upvals[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}

	else if (get_type(cl1) == module_c_closure && get_type(cl2) == module_c_wrap)
	{
	    lua_clonecfunction(state, 1);
		callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));

		callcheck::set(cl1, new_cclosure_handler);
		callcheck::wraps::set(cl1, cl2);
	}

	else if (get_type(cl1) == module_c_closure && get_type(cl2) == l_closure)
	{
		lua_clonecfunction(state, 1);
		callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));
		lua_ref(state, 2);

		callcheck::set(cl1, new_cclosure_handler);
		callcheck::wraps::set(cl1, cl2);
	}

	else if (get_type(cl1) == module_c_wrap && get_type(cl2) == roblox_c_closure)
	{
		if (nups1 >= nups2)
		{
			lua_clonecfunction(state, 1);
			callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));
			callcheck::wraps::set(clvalue(index2addr(state, -1)), callcheck::wraps::get(cl1));

			cl1->env = cl2->env;
			cl1->c.f = cl2->c.f.get();
			cl1->c.cont = cl2->c.cont.get();

			for (int i = 0; i < nups2; i++)
				setobj2n(state, &cl1->c.upvals[i], &cl2->c.upvals[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}

	else if (get_type(cl1) == module_c_wrap && get_type(cl2) == module_c_closure)
	{
		if (nups1 >= nups2)
		{
			lua_clonecfunction(state, 1);
			callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));
			callcheck::wraps::set(clvalue(index2addr(state, -1)), callcheck::wraps::get(cl1));

			callcheck::set(cl1, callcheck::get(cl2));

			cl1->env = cl2->env;
			cl1->c.cont = cl2->c.cont.get();

			for (int i = 0; i < nups2; i++)
				setobj2n(state, &cl1->c.upvals[i], &cl2->c.upvals[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}

	else if (get_type(cl1) == module_c_wrap && get_type(cl2) == l_closure)
	{
		lua_clonecfunction(state, 1);
		callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(cl1));
		callcheck::wraps::set(clvalue(index2addr(state, -1)), callcheck::wraps::get(cl1));

		lua_ref(state, 2);
		callcheck::wraps::set(cl1, cl2);
	}

	else if (get_type(cl1) == l_closure && (get_type(cl2) == roblox_c_closure || get_type(cl2) == module_c_closure))
	{
		//warning unsafe (debug.info will show function as variadic)

		lua_clonefunction(state, 1);
		const auto& spoof = MisakoRVM::compile(std::string(xorstr("local f = function() end; return f(...); ")).c_str());
		r_luau_load(reinterpret_cast<const std::uintptr_t>(state), cl1->l.p->source.get()->data, spoof.c_str(), spoof.size(), 0);

		Closure* clspoof = clvalue(index2addr(state, -1));
		setobj(state, &clspoof->l.p->k[0], index2addr(state, 2));
		clspoof->l.p->linedefined = cl1->l.p->linedefined;
		cl1->l.p = clspoof->l.p.get();

		lua_pop(state, 1);
	}

	else if (get_type(cl1) == l_closure && get_type(cl2) == module_c_wrap)
	{
		const Closure* l = callcheck::wraps::get(cl2);

		if (nups1 >= l->nupvalues)
		{
			lua_clonefunction(state, 1);

			cl1->env = l->env;
			cl1->l.p = l->l.p.get();

			for (int i = 0; i < l->nupvalues; i++)
				setobj2n(state, &cl1->l.uprefs[i], &l->l.uprefs[i]);
		}
		else
			luaL_error(state, "hookfunction - too many upvalues");
	}
	else
		luaL_error(state, "hookfunction - unsupported closure pair");

	return 1;
}

std::int32_t environment::load_bytecode(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);
	std::size_t input_size;
	std::size_t chunk_size;

	const auto input = luaL_checklstring(state, 1, &input_size);
	auto chunk_name = luaL_optlstring(state, 2, std::string("@").append(tools::io_addons::random_string(5)).c_str(), &chunk_size);

	const auto script = std::string(input, input_size);
	const auto compiledscript = MisakoRVM::compile(script);

	if (r_luau_load(reinterpret_cast<const std::uintptr_t>(state), chunk_name, compiledscript.c_str(), compiledscript.size(), 0))
	{
		lua_pushnil(state);
		lua_insert(state, -2);
		return 2;
	}

	return 1;
}

std::int32_t environment::check_caller(misako_State* state)
{
	const auto space = *reinterpret_cast<const std::uintptr_t*>(reinterpret_cast<const std::uintptr_t>(state) + extraspace_identity);
	const auto script = *reinterpret_cast<const std::uintptr_t*>(space + 52); // script ptr

	lua_pushboolean(state, !script);
	return 1;
}

std::int32_t environment::identify_executor(misako_State* state)
{
	lua_pushstring(state, "Misako 0.1");
	return 1;
}

std::int32_t environment::clone_ref(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TUSERDATA);

	// USE CONST AUTO BOGIE, CONST AUTO -rexi
	const auto original_userdata = uvalue(index2addr(state, 1));
	lua_newuserdatatagged(state, original_userdata->len, original_userdata->tag);

	const auto cloned_userdata = uvalue(index2addr(state, -1));
	cloned_userdata->metatable = original_userdata->metatable.get();

	//copy pointer and allignment
	*reinterpret_cast<std::uintptr_t*>(cloned_userdata->data) = *reinterpret_cast<std::uintptr_t*>(original_userdata->data);
	cloned_userdata->dummy.u = original_userdata->dummy.u;

	return 1;
}

std::int32_t environment::compare_instances(misako_State* state)
{
	lua_pushboolean(state, *reinterpret_cast<std::uintptr_t*>(uvalue(index2addr(state, 1))->data) == *reinterpret_cast<std::uintptr_t*>(uvalue(index2addr(state, 2))->data));
	return 1;
}

std::int32_t environment::clone_function(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TFUNCTION);

	switch (get_type(clvalue(index2addr(state, 1))))
	{
	case roblox_c_closure:
		lua_clonecfunction(state, 1);
		break;
	case module_c_closure:
		lua_clonecfunction(state, 1);
		callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(clvalue(index2addr(state, 1))));
		break;
	case module_c_wrap:
		lua_clonecfunction(state, 1);
		callcheck::set(clvalue(index2addr(state, -1)), callcheck::get(clvalue(index2addr(state, 1))));
		callcheck::wraps::set(clvalue(index2addr(state, -1)), callcheck::wraps::get(clvalue(index2addr(state, 1))));
		break;
	case l_closure:
		lua_clonefunction(state, 1);
		break;
	}

	return 1;
}

std::int32_t environment::is_our_closure(misako_State* state)
{
	// TO-DO: actually make this guys (tip for rexi in future: U can easily tag your closure there is empty space within closure struct)

	const auto closure_type = get_type(clvalue(index2addr(state, 1)));

	if (closure_type == module_c_closure || closure_type == module_c_wrap || closure_type == l_closure)
		lua_pushboolean(state, 1);
	else
		lua_pushboolean(state, 0);

	return 1;
}

std::int32_t environment::is_rbx_active(misako_State* state)
{
	const auto roblox_window = FindWindowW(0, std::wstring(xorstr(L"Roblox")).c_str());
	lua_pushboolean(state, GetForegroundWindow() == roblox_window);
	return 1;
}

std::int32_t environment::get_connections(misako_State* state) // table.foreach(getconnections(game:GetService("RunService").Stepped), print)
{
	luaL_checktype(state, 1, LUA_TUSERDATA);

	lua_getfield(state, 1, "connect");
	lua_pushvalue(state, 1);
	callcheck::lua_pushcclosure_(state, connection::blank_function, 0);
	lua_pcall(state, 2, 1, 0);

	const auto signal = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, -1));
	auto next = *reinterpret_cast<std::uintptr_t*>(signal + rbxscriptsignal_nextsignal); // sry the code was super rushed so i mightve done small retarded shit

	lua_createtable(state, 0, 0);
	auto count = 1;

	while (next != 0)
	{
		if (connections.count(next))
		{
			*reinterpret_cast<connection_struct*>(lua_newuserdata(state, sizeof(connection_struct), 0)) = connections[next];

			lua_createtable(state, 0, 0);
			callcheck::lua_pushcclosure_(state, connection::index_connection, 0);
			lua_setfield(state, -2, "__index");
			lua_pushstring(state, "table");
			lua_setfield(state, -2, "__type");
			lua_setmetatable(state, -2);
		}
		else
		{
			connection_struct new_signal;
			new_signal.signal_idx = next;
			new_signal.old_state = *reinterpret_cast<std::uintptr_t*>(next + rbxscriptsignal_state);

			*reinterpret_cast<connection_struct*>(lua_newuserdata(state, sizeof(connection_struct), 0)) = new_signal;

			lua_createtable(state, 0, 0);
			callcheck::lua_pushcclosure_(state, connection::index_connection, 0);
			lua_setfield(state, -2, "__index");
			lua_pushstring(state, "table");
			lua_setfield(state, -2, "__type");
			lua_setmetatable(state, -2);

			connections[next] = new_signal;
		}

		lua_rawseti(state, -2, count++);
		next = *reinterpret_cast<std::uintptr_t*>(next + rbxscriptsignal_nextsignal);
	}

	lua_getfield(state, -2, "Disconnect");
	lua_pushvalue(state, -3);
	lua_pcall(state, 1, 0, 0);
	return 1;
}

std::int32_t environment::get_roblox_path(misako_State* state)
{
	char File[MAX_PATH];
	GetModuleFileNameA(GetModuleHandleA(0), File, MAX_PATH);
	lua_pushstring(state, File);
	return 1;
}

/* Gets script bytecode but doesnt push to lua state *Used internally not in env. */
/* Note to myself (Rexi): if YARA sets the rule "found_lua" then you need to change decompiler NAME. */
std::string get_script_bytecode_str(misako_State* state) 
{
	luaL_checktype(state, 1, LUA_TUSERDATA);

	const auto script = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));
	const auto class_descriptor = *reinterpret_cast<std::uintptr_t*>(script + 0xC);
	const auto class_name = *reinterpret_cast<const char**>(class_descriptor + 0x4); /* Probably a better idea to check class id :) */

	const auto vftable = *reinterpret_cast<std::uintptr_t**>(script);
	const auto prot_string_getter = reinterpret_cast<std::uintptr_t(__thiscall*)(std::uintptr_t)>(vftable[51]);

	if (!std::strcmp(class_name, xorstr("ModuleScript")) || !std::strcmp(class_name, xorstr("LocalScript")))
	{
		const auto bytecode_data = *reinterpret_cast<std::string*>(*reinterpret_cast<std::uintptr_t*>(prot_string_getter(script) + 0x8) + 0x10);
		constexpr const char bytecode_magic[] = "RSB1";

		std::string input = bytecode_data;

		std::uint8_t hash_bytes[4];
		memcpy(hash_bytes, &input[0], 4);

		for (auto i = 0u; i < 4; ++i)
		{
			hash_bytes[i] ^= bytecode_magic[i];
			hash_bytes[i] -= i * 41;
		}

		for (size_t i = 0; i < input.size(); ++i)
			input[i] ^= hash_bytes[i % 4] + i * 41;

		XXH32(&input[0], input.size(), 42);

		std::uint32_t data_size;
		memcpy(&data_size, &input[4], 4);

		std::vector<std::uint8_t> data(data_size);

		ZSTD_decompress(&data[0], data_size, &input[8], input.size() - 8);

		return std::string(reinterpret_cast<char*>(&data[0]), data_size);
	}

	return "";
}

std::int32_t environment::get_script_bytecode(misako_State* state)
{
	const auto byte_code = get_script_bytecode_str(state);

	if (byte_code.empty())
		return 0;

	lua_pushlstring(state, byte_code.c_str(), byte_code.length());
	return 1;
}

std::int32_t environment::get_script_closure(misako_State* state)
{
	const auto byte_code = get_script_bytecode_str(state);

	if (byte_code.empty())
		return 0;

	r_luau_load(reinterpret_cast<const std::uintptr_t>(state), tools::io_addons::random_string(5).c_str(), byte_code.c_str(), byte_code.length(), 0);
	return 1;
}

std::int32_t environment::decompile_luau_bytecode(misako_State* state)
{
	const auto byte_code = get_script_bytecode_str(state);

	if (byte_code.empty())
		luaL_error(state, "decompiler argument 1 expected script or client script (local script, module script).");

	auto decompilation = luaU_decompile(byte_code);
	decompilation = xorstr("--Decompiled via Misako's Decompiler\n\n") + decompilation;

	lua_pushlstring(state, decompilation.c_str(), decompilation.size());
	return 1;
}

std::int32_t environment::dissassemble_luau_bytecode(misako_State* state)
{
	const auto byte_code = get_script_bytecode_str(state);

	if (byte_code.empty())
		luaL_error(state, "dissassembler argument 1 expected script or client script (local script, module script).");

	auto pos = 0u;
	auto curr_info = build_decompiler_info(byte_code, byte_code.size());
	std::string buffer = xorstr("--Disassembled via Misako's Disassembler\n\n");

	std::vector<std::shared_ptr <decompile_info>> appending_info;
	std::vector<std::shared_ptr <decompile_info>> currdecom_info;

	/* Kick off data. */
	auto whole = dissassemble_whole(curr_info, true);

	for (const auto& i : whole) 
	{
		buffer += std::to_string(pos) + ' ' + i->data + '\n';
		pos += i->size;
	}

	for (const auto& i : curr_info->proto.p)
		appending_info.emplace_back(i);

	do 
	{
		currdecom_info.clear();

		for (const auto& i : appending_info)
			currdecom_info.emplace_back(i);

		appending_info.clear();

		for (auto& i : currdecom_info) 
		{
			pos = 0u;
			whole = dissassemble_whole(i, true);

			for (const auto& i : whole) 
			{
				buffer += std::to_string(pos) + ' ' + i->data + '\n';
				pos += i->size;
			}

			for (const auto& i : i->proto.p)
				appending_info.emplace_back(i);
		}

	} while (appending_info.size());

	lua_pushlstring(state, buffer.c_str(), buffer.size());
	return 1;
}

std::int32_t environment::lz4_compress(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);

	const auto data = lua_tostring(state, 1);

	int dataSize = strlen(data);
	int maxSize = LZ4_compressBound(dataSize);

	std::vector<char> compressed(maxSize);
	int compressedSize = LZ4_compress(data, &compressed[0], dataSize);
	lua_pop(state, 1);

	lua_pushlstring(state, compressed.data(), compressedSize);
	return 1;
}

std::int32_t environment::lz4_decompress(misako_State* state)
{
	luaL_checktype(state, 1, LUA_TSTRING);
	luaL_checktype(state, 2, LUA_TNUMBER);

	const auto data = lua_tostring(state, 1);
	const auto decompressedSize = lua_tonumber(state, 2);

	std::vector<char> decompressed(decompressedSize);
	LZ4_decompress_safe(data, &decompressed[0], strlen(data), decompressedSize);

	lua_pushlstring(state, decompressed.data(), decompressedSize);
	return 1;
}

static const luaL_Reg custom_environment_functions[] = {
	{"getgenv", environment::get_genv},
	{"getrenv", environment::get_renv},
	{"gettenv", environment::get_tenv},
	{"getreg", environment::get_reg},
	{"getgc", environment::get_gc},

	{"getthreadidentity", environment::get_thread_identity},
	{"setthreadidentity", environment::set_thread_identity},

	{"getfflag", environment::get_fast_flag},
	{"setfflag", environment::set_fast_flag},

	{"getclipboard", environment::get_clipboard},
	{"setclipboard", environment::set_clipboard},

	{"getrawmetatable", environment::get_raw_metatable},
	{"setrawmetatable", environment::set_raw_metatable},

	{"getnamecallmethod", environment::get_namecall_method},
	{"setnamecallmethod", environment::set_namecall_method},

	{"getndm", environment::get_networked_datamodel},
	{"setndm", environment::set_networked_datamodel},

	{"getproperties", environment::get_all_properties},
	{"gethiddenproperties", environment::get_hidden_properties},

	{"isscriptable", environment::is_scriptable},
	{"setscriptable", environment::set_scriptable},

	{"getfpscap", environment::get_fps_cap},
	{"setfpscap", environment::set_fps_cap},

	{"setreadonly", environment::set_readonly},
	{"isreadonly", environment::is_readonly},

	{"iscclosure", environment::is_cclosure},
	{"islclosure", environment::is_lclosure},

	{"fireclickdetector", environment::fire_clickdetector},
	{"firetouchinterest", environment::fire_touchtransmitter},
	{"fireproximityprompt", environment::fire_proximityprompt},

	{"cloneref", environment::clone_ref},
	{"clonefunction", environment::clone_function},
	{"compareinstances", environment::compare_instances},

	{"lz4compress", environment::lz4_compress},
	{"lz4decompress", environment::lz4_decompress},

	{"decompile", environment::decompile_luau_bytecode},
	{"disassemble", environment::dissassemble_luau_bytecode},

	{"isrbxactive", environment::is_rbx_active},
	{"isourclosure", environment::is_our_closure},

	{"gethui", environment::get_hidden_gui},
	{"getrobloxpath", environment::get_roblox_path},
	{"getconnections", environment::get_connections},
	{"getscriptclosure", environment::get_script_closure},
	{"getscriptbytecode", environment::get_script_bytecode},

	{"newcclosure", environment::new_cclosure},
	{"messageboxasync", environment::messagebox_async},
	{"queue_on_teleport", environment::queue_on_teleport},
	{"request", environment::http_request},
	{"hookfunction", environment::hook_function},
	{"loadstring", environment::load_bytecode},
	{"checkcaller", environment::check_caller},
	{"identifyexecutor", environment::identify_executor},

	{NULL, NULL}
};

void environment::setup_custom_environment(misako_State* state)
{
	auto to_register = custom_environment_functions;

	for (; to_register->name; to_register++)
		misako_register(state, to_register->func, to_register->name);
}