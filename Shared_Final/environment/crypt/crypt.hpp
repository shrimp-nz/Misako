#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

#include <dllmain.hpp>

namespace crypt_environment
{
	std::int32_t encrypt(misako_State* state);
	std::int32_t decrypt(misako_State* state);

	namespace base64 
	{
		std::int32_t encode(misako_State* state);
		std::int32_t decode(misako_State* state);
	}

	std::int32_t hash(misako_State* state);
	std::int32_t derive(misako_State* state);
	std::int32_t random(misako_State* state);

	namespace custom 
	{
		std::int32_t encrypt(misako_State* state);
		std::int32_t decrypt(misako_State* state);
		std::int32_t hash(misako_State* state);
	}

	void setup_crypt_environment(misako_State* state);
}
