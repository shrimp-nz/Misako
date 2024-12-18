#include "connection.hpp"
#include <roblox/offsets.hpp>
#include <bypasses/callcheck/callcheck.hpp>
#include <dependencies/luau/VM/src/lualib.h>

std::unordered_map<std::uintptr_t, connection_struct> connections;

std::int32_t connection::index_connection(misako_State* state)
{
	std::string key = std::string(luaL_checklstring(state, 2, nullptr));

	std::transform(key.begin(), key.end(), key.begin(),
		[](std::uint8_t c) { return std::tolower(c); });

	if (!std::strcmp(key.c_str(), "enable"))
	{
		lua_pushvalue(state, 1);
		callcheck::lua_pushcclosure_(state, enable_connection, 0);
	}
	else if (!std::strcmp(key.c_str(), "disable"))
	{
		lua_pushvalue(state, 1);
		callcheck::lua_pushcclosure_(state, disable_connection, 0);
	}
	else if (!std::strcmp(key.c_str(), "function"))
	{
		const auto connection = *reinterpret_cast<connection_struct*>(lua_touserdata(state, 1));

		lua_getref(state, *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(connection.signal_idx + 0x1C) + 0x3C) + 0xC));

		if (lua_type(state, -1) != LUA_TFUNCTION)
			callcheck::lua_pushcclosure_(state, blank_function, 0);
	}
	else if (!std::strcmp(key.c_str(), "fire"))
	{
		callcheck::lua_pushcclosure_(state, fire_connection, 0);
	}
	else if (!std::strcmp(key.c_str(), "thread"))
	{
		// TO-DO: make this
		// what it does: return the thread of the connection
	}
	else if (!std::strcmp(key.c_str(), "enabled"))
	{
		const auto connection = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));
		const auto enabled = *reinterpret_cast<std::uintptr_t*>(connection + rbxscriptsignal_state);

		lua_pushboolean(state, enabled);
	}
	else if (!std::strcmp(key.c_str(), "disconnect"))
	{
		lua_getglobal(state, "disconnect");
		lua_pcall(state, 0, 0, 0);
	}
	else if (!std::strcmp(key.c_str(), "luaconnection"))
	{
		// TO-DO: verify this works
		const auto connection = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));
		const auto connection_state = *reinterpret_cast<std::uintptr_t*>(connection + 5);

		lua_pushboolean(state, connection_state != 0);
	}
	else if (!std::strcmp(key.c_str(), "foreignstate"))
	{
		// TO-DO: verify this works
		lua_pushboolean(state, states::lua_state->global == state->global);
	}
	else
		luaL_error(state, "invalid connection method");

	return 1;
}

std::int32_t connection::enable_connection(misako_State* state)
{
	const auto signal = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));

	if (!connections.count(signal))
	{
		connection_struct connection_values;
		connection_values.signal_idx = signal;
		connection_values.old_state = *reinterpret_cast<std::uintptr_t*>(signal + rbxscriptsignal_state);

		connections[signal] = connection_values;
	}

	*reinterpret_cast<std::uintptr_t*>(signal + rbxscriptsignal_state) = connections[signal].old_state;
	return 0;
}

std::int32_t connection::disable_connection(misako_State* state)
{
	const auto signal = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));

	if (!connections.count(signal))
	{
		connection_struct connection_values;
		connection_values.signal_idx = signal;
		connection_values.old_state = *reinterpret_cast<std::uintptr_t*>(signal + rbxscriptsignal_state);

		connections[signal] = connection_values;
	}

	*reinterpret_cast<std::uintptr_t*>(signal + rbxscriptsignal_state) = 0;
	return 0;
}

std::int32_t connection::fire_connection(misako_State* state)
{
	const auto connection = *reinterpret_cast<connection_struct*>(lua_touserdata(state, 1));
	lua_remove(state, 1);

	int args = lua_gettop(state);
	lua_getref(state, *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(connection.signal_idx + 0x1C) + 0x3C) + 0xC));

	lua_insert(state, -(args)-1);

	if (lua_pcall(state, args, -1, 0))
	{
		if (!strcmp(lua_tostring(state, -1), "attempt to yield across metamethod/C-call boundary"))
			return lua_yield(state, 0);
		else
			luaL_error(state, lua_tostring(state, -1));
	}

	return lua_gettop(state);
}

std::int32_t connection::blank_function(misako_State* state)
{
	return 0;
}