#include <Windows.h>
#include <vector>

#include "callcheck.hpp"
#include <roblox/offsets.hpp>

namespace callcheck
{
	std::map<Closure*, lua_CFunction> cfunction_map;
	std::map<Closure*, Closure*> nccmap;

	std::int32_t cfunction_handler(misako_State* rL)
	{
		const auto ci = (rL->ci);

		if (ci - rL->base_ci > 0)
		{
			const auto closure = clvalue(ci->func);
			const auto found = cfunction_map.find(closure);

			if (found != cfunction_map.end())
				return found->second(rL);
		}
		return 0;
	}

	__declspec(naked) void retcheck_hook()
	{
		__asm
		{
			// callcheck support
			pop eax; // pop the 0 or misako_State
			cmp eax, 0;
			je retcheck;
			push eax; // it was misako_State, repush
			jmp cfunction_handler; // call the dispatch to our own closures (uncomment this line for c closure handling too)

		retcheck:
			pop edi;
			pop esi;
			pop ebx;

			add esp, 0x10;

			pop eax;
			mov fs : [0] , eax;

			add esp, 8;

			mov esp, ebp;
			pop ebp;

			ret;
		}
	}

	void bypass()
	{
		DWORD old_protect, new_protect;
		VirtualProtect(reinterpret_cast<void*>(ret_hook_address), 5, PAGE_EXECUTE_READWRITE, &old_protect);
		*reinterpret_cast<std::uint8_t*>(ret_hook_address) = 0xE9;
		*reinterpret_cast<std::uintptr_t*>(ret_hook_address + 1) = reinterpret_cast<std::uintptr_t>(retcheck_hook) - ret_hook_address - 5;
		VirtualProtect(reinterpret_cast<void*>(ret_hook_address), 5, old_protect, &new_protect);
	}

	lua_CFunction get(Closure* c)
	{
		return cfunction_map.find(c)->second;
	}

	void set(Closure* c, lua_CFunction fn)
	{
		cfunction_map[c] = fn;
	}

	Closure* wraps::get(Closure* c)
	{
		return nccmap.find(c)->second;
	}

	void wraps::set(Closure* c, Closure* l)
	{
		nccmap[c] = l;
	}

	void lua_pushcclosure_(misako_State* rL, lua_CFunction fn, std::int32_t nup)
	{
		lua_pushcclosure(rL, reinterpret_cast<lua_CFunction>(ret_push_address), 0, nup, 0);
		Closure* closure = *reinterpret_cast<Closure**>(index2addr(rL, -1));
		cfunction_map[closure] = fn;
	}

	std::map<Closure*, lua_CFunction>* getfmap()
	{
		return &cfunction_map;
	}

	std::map<Closure*, Closure*>* getnccmap()
	{
		return &nccmap;
	}
}

void misako_register(misako_State* rL, lua_CFunction fn, const char* name)
{
	callcheck::lua_pushcclosure_(rL, fn, 0);
	lua_setfield(rL, -10002, name);
}

void misako_register_table(misako_State* rL, lua_CFunction fn, const char* name)
{
	callcheck::lua_pushcclosure_(rL, fn, 0);
	lua_setfield(rL, -2, name);
}