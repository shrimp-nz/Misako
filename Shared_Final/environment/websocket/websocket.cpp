#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>

#include <bypasses/callcheck/callcheck.hpp>
#include <lua.h>
#include <lualib.h>

#include "websocket.hpp"

namespace websocket_environment
{
	std::string ws_msg_cache = "";
	
	struct module_socket 
	{
		bool connected;
		misako_State* thread;
		int thread_ref;
		ix::WebSocket* ws;
		int event_message;
		int event_close;
		int send;
		int close;
	};

	std::int32_t index(misako_State* state)
	{
		luaL_checktype(state, 1, LUA_TUSERDATA);
		luaL_checktype(state, 2, LUA_TSTRING);

		const auto socket = reinterpret_cast<module_socket*>(lua_touserdata(state, 1));
		const auto key = lua_tostring(state, 2);

		if (!socket->connected)
			luaL_error(state, "Websocket - cannot use a dead socket");

		if (!std::strcmp(key, "Send"))
		{
			lua_getref(state, socket->send);
			return 1;
		}
		else if (!std::strcmp(key, "Close"))
		{
			lua_getref(state, socket->close);
			return 1;
		}
		else if (!std::strcmp(key, "OnMessage"))
		{
			lua_getref(state, socket->event_message);
			lua_getfield(state, -1, "Event");
			return 1;
		}
		else if (!std::strcmp(key, "OnClose"))
		{
			lua_getref(state, socket->event_close);
			lua_getfield(state, -1, "Event");
			return 1;
		}
		else
			luaL_error(state, "invalid websocket key");

		return 0;
	}

	std::int32_t connect(misako_State* state)
	{
		luaL_checktype(state, 1, LUA_TSTRING);

		std::string url(lua_tostring(state, 1));
		void* ud = lua_newuserdata(state, sizeof(module_socket));

		const auto socket = reinterpret_cast<module_socket*>(ud);
		*socket = module_socket{};

		socket->thread = lua_newthread(state);
		socket->thread_ref = lua_ref(state, -1);
		socket->ws = new ix::WebSocket();
		socket->ws->setUrl(url);

		lua_pop(state, 1);

		socket->ws->setOnMessageCallback([socket](const ix::WebSocketMessagePtr& msg)
			{
				if (msg->type == ix::WebSocketMessageType::Message)
				{
					ws_msg_cache = msg->str;

					lua_getref(socket->thread, socket->event_message);
					lua_getfield(socket->thread, -1, "Fire");
					lua_pushvalue(socket->thread, -2);
					lua_pushlstring(socket->thread, ws_msg_cache.c_str(), ws_msg_cache.size());
					lua_pcall(socket->thread, 2, 0, 0);
					lua_pop(socket->thread, 1);
				}

				if (msg->type == ix::WebSocketMessageType::Close || msg->type == ix::WebSocketMessageType::Error)
				{
					socket->connected = false;

					lua_getref(socket->thread, socket->event_close);
					lua_getfield(socket->thread, -1, "Fire");
					lua_pushvalue(socket->thread, -2);
					lua_pcall(socket->thread, 1, 0, 0);
					lua_pop(socket->thread, 1);

					lua_unref(socket->thread, socket->send);
					lua_unref(socket->thread, socket->close);
					lua_unref(socket->thread, socket->event_message);
					lua_unref(socket->thread, socket->event_close);
					lua_unref(socket->thread, socket->thread_ref);

					ws_msg_cache.erase();
				}
			}
		);

		luaL_getmetatable(state, "4b6dd0d7b76c");
		lua_setmetatable(state, -2);

		callcheck::lua_pushcclosure_(state, send, 0);
		socket->send = lua_ref(state, -1);
		lua_pop(state, 1);

		callcheck::lua_pushcclosure_(state, close, 0);
		socket->close = lua_ref(state, -1);
		lua_pop(state, 1);

		lua_getglobal(state, "Instance");
		lua_getfield(state, -1, "new");
		lua_pushstring(state, "BindableEvent");
		lua_pcall(state, 1, -1, 0);
		socket->event_message = lua_ref(state, -1);
		lua_pop(state, 2);

		lua_getglobal(state, "Instance");
		lua_getfield(state, -1, "new");
		lua_pushstring(state, "BindableEvent");
		lua_pcall(state, 1, -1, 0);
		socket->event_close = lua_ref(state, -1);
		lua_pop(state, 2);

		socket->ws->connect(5);

		if (socket->ws->getReadyState() != ix::ReadyState::Open)
			luaL_error(state, "Websocket - connection failed");

		socket->connected = true;
		socket->ws->start();

		return 1;
	}

	std::int32_t send(misako_State* state)
	{
		luaL_checktype(state, 1, LUA_TUSERDATA);
		luaL_checktype(state, 2, LUA_TSTRING);

		const auto socket = reinterpret_cast<module_socket*>(lua_touserdata(state, 1));

		std::size_t len;
		const auto cstr = lua_tolstring(state, 2, &len);
		std::string data(cstr, len);

		socket->ws->send(data, true);
	}

	int close(misako_State* state)
	{
		luaL_checktype(state, 1, LUA_TUSERDATA);

		const auto socket = reinterpret_cast<module_socket*>(lua_touserdata(state, 1));

		socket->ws->stop();
		return 0;
	}

	void setup_websocket_environment(misako_State* state)
	{
		ix::initNetSystem();

		luaL_newmetatable(state, "4b6dd0d7b76c");
		callcheck::lua_pushcclosure_(state, index, 0);
		lua_setfield(state, -2, "__index");
		lua_setreadonly(state, -1, true);
		lua_pop(state, 1);

		lua_newtable(state);
		misako_register_table(state, websocket_environment::connect, "connect");
		lua_setglobal(state, "websocket");
	}
}