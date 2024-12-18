#pragma once
#include <Windows.h>
#include <cstdint>
#include <map>

#include <dllmain.hpp>
#include <dependencies/xorstring/xorstring.hpp>



namespace callcheck
{
	extern std::map<Closure*, lua_CFunction> cfunction_map;
	extern std::map<Closure*, Closure*> nccmap;

	extern std::map<Closure*, lua_CFunction>* getfmap();
	extern std::map<Closure*, Closure*>* getnccmap();

	void bypass();
	void lua_pushcclosure_(misako_State* rL, lua_CFunction fn, std::int32_t nup = 0);

	lua_CFunction get(Closure* c);
	void set(Closure* c, lua_CFunction fn);

	namespace wraps
	{
		Closure* get(Closure* c);
		void set(Closure* c, Closure* l);
	}
}

extern void misako_register(misako_State* rL, lua_CFunction fn, const char* name);
extern void misako_register_table(misako_State* rL, lua_CFunction fn, const char* name);