#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

#include "dependencies/imgui/imgui.h"
#include <dllmain.hpp>

enum drawingenum
{
	text,
	line,
	square,
	triangle,
	circle,
	quad,
	image
};

class base_t
{
public:
	bool visible = false;
	int zindex = 1;
	drawingenum type;
};

class line_t : public base_t
{
public:
	ImVec2 from = { 0.f, 0.f };
	ImVec2 to = { 0.f, 0.f };
	ImVec4 color = { 0.f, 0.f, 0.f, 0.f };

	int thickness = 16;
	float transparency = 1.f;
};

class circle_t : public base_t
{
public:
	ImVec2 position = { 0.f, 0.f };
	ImVec4 color = { 0.f, 0.f, 0.f, 0.f };

	bool filled = false;
	float transparency = 1.f;

	int radius = 0;
	int thickness = 15;
	int numsides = 250;
};

class square_t : public base_t
{
public:
	ImVec2 position = { 0.f, 0.f };
	ImVec2 size = { 0.f, 0.f };
	ImVec4 color = { 0.f, 0.f, 0.f, 0.f };

	int thickness = 16;

	bool filled = false;

	float transparency = 1.f;
};

class triangle_t : public base_t
{
public:
	ImVec2 pointa = { 0.f, 0.f };
	ImVec2 pointb = { 0.f, 0.f };
	ImVec2 pointc = { 0.f, 0.f };

	ImVec4 color = { 0.f, 0.f, 0.f, 0.f };

	int thickness = 15;

	bool filled = false;

	float transparency = 1.f;
};

class quad_t : public base_t
{
public:
	ImVec2 pointa = { 0.f, 0.f };
	ImVec2 pointb = { 0.f, 0.f };
	ImVec2 pointc = { 0.f, 0.f };
	ImVec2 pointd = { 0.f, 0.f };

	ImVec4 color = { 0.f, 0.f, 0.f, 0.f };

	int thickness = 16;

	bool filled = false;

	float transparency = 1.f;
};

class text_t : public base_t
{
public:
	char text[512];

	ImVec2 position = { 0.f, 0.f };
	ImVec4 color = { 0.f, 0.f, 0.f, 0.f };

	int font = 0;

	bool center = false;
	bool outline = false;

	float size = 16;
	float transparency = 1.f;

	ImVec2 textbounds = { 0.f, 16.f };
};

class image_t : public base_t
{
public:
	char data[512];

	ImVec2 position = {0.f, 0.f};
	ImVec4 color = {0.f, 0.f, 0.f, 0.f};
	ImVec2 size = {0.f, 0.f};

	float transparency = 1.f;
	int rounding = 0;
};

extern std::vector<base_t*> drawing_vector;

namespace drawing_environment
{
	std::int32_t drawing_new(misako_State* rL);

	std::int32_t is_render_property(misako_State* state);
	std::int32_t get_render_property(misako_State* state);
	std::int32_t set_render_property(misako_State* rL);

	extern void setup_drawing_environment(misako_State* rL);
}