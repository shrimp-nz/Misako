#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

#include <dllmain.hpp>

namespace input_environment
{
	std::int32_t mouse1_click(misako_State* state);
	std::int32_t mouse1_press(misako_State* state);
	std::int32_t mouse1_release(misako_State* state);

	std::int32_t mouse2_click(misako_State* state);
	std::int32_t mouse2_press(misako_State* state);
	std::int32_t mouse2_release(misako_State* state);

	std::int32_t mouse_moverel(misako_State* state);
	std::int32_t mouse_moveabs(misako_State* state);
	std::int32_t mouse_scroll(misako_State* state);

	std::int32_t left_click(misako_State* state);
	std::int32_t right_click(misako_State* state);

	std::int32_t key_press(misako_State* state);
	std::int32_t key_tap(misako_State* state);
	std::int32_t key_release(misako_State* state);

	void setup_input_environment(misako_State* state);
}