#pragma once

namespace websocket_environment
{
	extern std::string ws_msg_cache;
	extern struct module_socket;
	
	std::int32_t index(misako_State* state);

	std::int32_t connect(misako_State* state);
	std::int32_t send(misako_State* state);
	std::int32_t close(misako_State* state);

	void setup_websocket_environment(misako_State* state);
}