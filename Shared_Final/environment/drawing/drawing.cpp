#include "drawing.hpp"

#include <string>
#include <format>

#include "bypasses/callcheck/callcheck.hpp"
#include <lualib.h>

std::vector<base_t*> drawing_vector;

struct color_t
{
	float r, g, b;

	operator ImVec4() { return { r, g, b, 1.f }; }

	operator ImVec4() const { return { r, g, b, 1.f }; }
};

void makevec2(misako_State* state, const ImVec2& vector)
{
	lua_getfield(state, -10002, "Vector2");
	lua_getfield(state, -1, "new");

	lua_pushnumber(state, vector.x);
	lua_pushnumber(state, vector.y);

	lua_pcall(state, 2, 1, 0);
}

ImVec2 tovec2(misako_State* state, std::int32_t index)
{
	return *reinterpret_cast<const ImVec2*>(lua_topointer(state, index));
}

void makecol3(misako_State* state, const ImVec4& color)
{
	lua_getfield(state, -10002, "Color3");
	lua_getfield(state, -1, "new");

	lua_pushnumber(state, color.x);
	lua_pushnumber(state, color.y);
	lua_pushnumber(state, color.z);

	lua_pcall(state, 3, 1, 0);
}

ImVec4 tocol3(misako_State* state, std::int32_t Index)
{
	return *reinterpret_cast<const color_t*>(lua_topointer(state, Index));
}

namespace drawing_base
{
	std::int32_t remove(misako_State* state)
	{
		const auto item = reinterpret_cast<base_t*>(lua_touserdata(state, 1));
		if (const auto iter = std::find(drawing_vector.begin(), drawing_vector.end(), item); iter != drawing_vector.end())
			drawing_vector.erase(iter);

		return 0;
	}

	std::int32_t clear(misako_State* state)
	{
		drawing_vector.clear();
		return 0;
	}

	void index(misako_State* state, base_t* object, const std::string& drawing_object)
	{
		if (!std::strcmp(drawing_object.c_str(), xorstr("Visible")))
			lua_pushboolean(state, object->visible);

		else if (!std::strcmp(drawing_object.c_str(), xorstr("ZIndex")))
			lua_pushboolean(state, object->zindex);

		else if (!std::strcmp(drawing_object.c_str(), xorstr("Remove")))
			callcheck::lua_pushcclosure_(state, remove, 0);

		else if (!std::strcmp(drawing_object.c_str(), xorstr("Destroy")))
			callcheck::lua_pushcclosure_(state, remove, 0);
	}

	void newindex(misako_State* state, base_t* object, const std::string& drawing_object)
	{
		if (!std::strcmp(drawing_object.c_str(), xorstr("Visible")))
			object->visible = lua_toboolean(state, 3);

		else if (!std::strcmp(drawing_object.c_str(), xorstr("ZIndex")))
			object->zindex = lua_tointeger(state, 3);
	}
}

namespace drawing_line
{
	std::int32_t index(misako_State* state)
	{
		const auto obj = reinterpret_cast<line_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("From")))
			makevec2(state, obj->from);

		else if (!std::strcmp(drawing_object, xorstr("To")))
			makevec2(state, obj->to);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			makecol3(state, obj->color);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			lua_pushnumber(state, obj->thickness);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			lua_pushnumber(state, obj->transparency);
		else
			drawing_base::index(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}

	std::int32_t newindex(misako_State* state)
	{
		const auto obj = reinterpret_cast<line_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("From")))
			obj->from = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("To")))
			obj->to = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			obj->color = tocol3(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			obj->thickness = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			obj->transparency = lua_tonumber(state, 3);
		else
			drawing_base::newindex(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 0;
	}
}

namespace drawing_circle
{
	std::int32_t index(misako_State* state)
	{
		const auto obj = reinterpret_cast<circle_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("Position")))
			makevec2(state, obj->position);

		else if (!std::strcmp(drawing_object, xorstr("Radius")))
			lua_pushnumber(state, obj->radius);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			makecol3(state, obj->color);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			lua_pushnumber(state, obj->thickness);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			lua_pushboolean(state, obj->filled);

		else if (!std::strcmp(drawing_object, xorstr("NumSides")))
			lua_pushnumber(state, obj->numsides);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			lua_pushnumber(state, obj->transparency);
		else
			drawing_base::index(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}

	std::int32_t newindex(misako_State* state)
	{
		const auto obj = reinterpret_cast<circle_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("Position")))
			obj->position = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Radius")))
			obj->radius = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			obj->color = tocol3(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			obj->thickness = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			obj->filled = lua_toboolean(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("NumSides")))
			obj->numsides = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			obj->transparency = lua_tonumber(state, 3);
		else
			drawing_base::newindex(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 0;
	}
}
namespace drawing_square
{
	std::int32_t index(misako_State* state)
	{
		const auto obj = reinterpret_cast<square_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("Position")))
			makevec2(state, obj->position);

		else if (!std::strcmp(drawing_object, xorstr("Size")))
			makevec2(state, obj->size);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			makecol3(state, obj->color);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			lua_pushnumber(state, obj->thickness);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			lua_pushnumber(state, obj->transparency);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			lua_pushboolean(state, obj->filled);

		else
			drawing_base::index(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}

	std::int32_t newindex(misako_State* state)
	{
		const auto obj = reinterpret_cast<square_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("Position")))
			obj->position = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Size")))
			obj->size = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			obj->color = tocol3(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			obj->thickness = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			obj->transparency = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			obj->filled = lua_toboolean(state, 3);

		else
			drawing_base::newindex(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 0;
	}
}

namespace drawing_triangle
{
	std::int32_t index(misako_State* state)
	{
		const auto obj = reinterpret_cast<triangle_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("PointA")))
			makevec2(state, obj->pointa);

		else if (!std::strcmp(drawing_object, xorstr("PointB")))
			makevec2(state, obj->pointb);

		else if (!std::strcmp(drawing_object, xorstr("PointC")))
			makevec2(state, obj->pointc);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			makecol3(state, obj->color);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			lua_pushnumber(state, obj->thickness);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			lua_pushnumber(state, obj->transparency);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			lua_pushboolean(state, obj->filled);

		else
			drawing_base::index(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}

	std::int32_t newindex(misako_State* state)
	{
		const auto obj = reinterpret_cast<triangle_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object,  ("PointA")))
			obj->pointa = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("PointB")))
			obj->pointb = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("PointC")))
			obj->pointc = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			obj->color = tocol3(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			obj->thickness = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			obj->transparency = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			obj->filled = lua_toboolean(state, 3);

		else
			drawing_base::newindex(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 0;
	}
}

namespace drawing_quad
{
	std::int32_t index(misako_State* state)
	{
		const auto obj = reinterpret_cast<quad_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("PointA")))
			makevec2(state, obj->pointa);

		else if (!std::strcmp(drawing_object, xorstr("PointB")))
			makevec2(state, obj->pointb);

		else if (!std::strcmp(drawing_object, xorstr("PointC")))
			makevec2(state, obj->pointc);

		else if (!std::strcmp(drawing_object, xorstr("PointD")))
			makevec2(state, obj->pointd);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			makecol3(state, obj->color);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			lua_pushnumber(state, obj->thickness);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			lua_pushnumber(state, obj->transparency);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			lua_pushboolean(state, obj->filled);

		else
			drawing_base::index(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}

	std::int32_t newindex(misako_State* state)
	{
		const auto obj = reinterpret_cast<quad_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("PointA")))
			obj->pointa = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("PointB")))
			obj->pointb = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("PointC")))
			obj->pointc = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("PointD")))
			obj->pointd = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			obj->color = tocol3(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Thickness")))
			obj->thickness = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			obj->transparency = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Filled")))
			obj->filled = lua_toboolean(state, 3);

		else
			drawing_base::newindex(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 0;
	}
}

namespace drawing_text
{
	std::int32_t index(misako_State* state)
	{
		const auto obj = reinterpret_cast<text_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("Text")))
			lua_pushstring(state, obj->text);

		else if (!std::strcmp(drawing_object, xorstr("Position")))
			makevec2(state, obj->position);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			makecol3(state, obj->color);

		else if (!std::strcmp(drawing_object, xorstr("Center")))
			lua_pushboolean(state, obj->center);

		else if (!std::strcmp(drawing_object, xorstr("Outline")))
			lua_pushboolean(state, obj->outline);

		else if (!std::strcmp(drawing_object, xorstr("Size")))
			lua_pushnumber(state, obj->size);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			lua_pushnumber(state, obj->transparency);

		else if (!std::strcmp(drawing_object, xorstr("Font")))
			lua_pushnumber(state, obj->font);

		else if (!std::strcmp(drawing_object, xorstr("TextBounds")))
			makevec2(state, obj->textbounds);

		else
			drawing_base::index(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}

	std::int32_t newindex(misako_State* state)
	{
		const auto obj = reinterpret_cast<text_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, xorstr("Text")))
		{
			const char* str = luaL_checklstring(state, 3, nullptr);
			std::strcpy(obj->text, str);
		}

		else if (!std::strcmp(drawing_object, xorstr("Position")))
			obj->position = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Color")))
			obj->color = tocol3(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Center")))
			obj->center = lua_toboolean(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Outline")))
			obj->outline = lua_toboolean(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Size")))
			obj->size = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Transparency")))
			obj->transparency = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("Font")))
			obj->font = lua_tonumber(state, 3);

		else if (!std::strcmp(drawing_object, xorstr("TextBounds")))
			obj->textbounds = tovec2(state, 3);

		else
			drawing_base::newindex(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 0;
	}
}

namespace drawing_image
{
	std::int32_t index(misako_State* state)
	{
		const auto obj = reinterpret_cast<image_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, "Data"))
			lua_pushstring(state, obj->data);

		else if (!std::strcmp(drawing_object,"Position"))
			makevec2(state, obj->position);

		else if (!std::strcmp(drawing_object, "Color"))
			makecol3(state, obj->color);

		else if (!std::strcmp(drawing_object, "Size"))
			makevec2(state, obj->size);

		else if (!std::strcmp(drawing_object, "Rounding"))
			lua_pushnumber(state, obj->rounding);

		else if (!std::strcmp(drawing_object, "Transparency"))
			lua_pushnumber(state, obj->transparency);

		else
			drawing_base::index(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}

	std::int32_t newindex(misako_State* state)
	{
		const auto obj = reinterpret_cast<image_t*>(lua_touserdata(state, 1));
		const auto drawing_object = lua_tolstring(state, 2, nullptr);

		if (!std::strcmp(drawing_object, "Data"))
		{
			const char* str = luaL_checklstring(state, 3, nullptr);
			std::strcpy(obj->data, str);
		}

		else if (!std::strcmp(drawing_object, "Position"))
			obj->position = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, "Color"))
			obj->color = tocol3(state, 3);

		else if (!std::strcmp(drawing_object, "Size"))
			obj->size = tovec2(state, 3);

		else if (!std::strcmp(drawing_object, "Rounding"))
			obj->rounding = lua_tonumber(state, 3);

		else
			drawing_base::newindex(state, dynamic_cast<base_t*>(obj), drawing_object);

		return 1;
	}
}

std::int32_t drawing_environment::drawing_new(misako_State* state)
{
	const auto drawing_object = lua_tolstring(state, 1, nullptr);

	if (!std::strcmp(drawing_object, xorstr("Line")))
	{
		const auto ud = reinterpret_cast<line_t*>(lua_newuserdata(state, sizeof(line_t), 0));

		*ud = line_t{};
		ud->type = line;

		lua_getfield(state, LUA_REGISTRYINDEX, "drawline");
		lua_setmetatable(state, -2);

		drawing_vector.push_back(ud);
	}
	else if (!std::strcmp(drawing_object, xorstr("Circle")))
	{
		const auto ud = reinterpret_cast<circle_t*>(lua_newuserdata(state, sizeof(circle_t), 0));

		*ud = circle_t{};
		ud->type = circle;

		lua_getfield(state, LUA_REGISTRYINDEX, "drawcircle");
		lua_setmetatable(state, -2);

		drawing_vector.push_back(ud);
	}
	else if (!std::strcmp(drawing_object, xorstr("Square")))
	{
		const auto ud = reinterpret_cast<square_t*>(lua_newuserdata(state, sizeof(square_t), 0));

		*ud = square_t{};
		ud->type = square;

		lua_getfield(state, LUA_REGISTRYINDEX, "drawsquare");
		lua_setmetatable(state, -2);

		drawing_vector.push_back(ud);
	}
	else if (!std::strcmp(drawing_object, xorstr("Triangle")))
	{
		const auto ud = reinterpret_cast<triangle_t*>(lua_newuserdata(state, sizeof(triangle_t), 0));

		*ud = triangle_t{};
		ud->type = triangle;

		lua_getfield(state, LUA_REGISTRYINDEX, "drawtriangle");
		lua_setmetatable(state, -2);

		drawing_vector.push_back(ud);
	}
	else if (!std::strcmp(drawing_object, xorstr("Quad")))
	{
		const auto ud = reinterpret_cast<quad_t*>(lua_newuserdata(state, sizeof(quad_t), 0));

		*ud = quad_t{};
		ud->type = quad;

		lua_getfield(state, LUA_REGISTRYINDEX, "drawquad");
		lua_setmetatable(state, -2);

		drawing_vector.push_back(ud);
	}
	else if (!std::strcmp(drawing_object, xorstr("Text")))
	{
		const auto ud = reinterpret_cast<text_t*>(lua_newuserdata(state, sizeof(text_t), 0));

		*ud = text_t{};
		ud->type = text;

		lua_getfield(state, LUA_REGISTRYINDEX, "drawtext");
		lua_setmetatable(state, -2);

		drawing_vector.push_back(ud);
	}
	else if (!std::strcmp(drawing_object, xorstr("Image")))
	{
		const auto ud = reinterpret_cast<image_t*>(lua_newuserdata(state, sizeof(image_t), 0));

		*ud = image_t{};
		ud->type = image;

		lua_getfield(state, LUA_REGISTRYINDEX, "drawimage");
		lua_setmetatable(state, -2);

		drawing_vector.push_back(ud);
	}
	else
	{
		luaL_errorL(state, std::format("{} is an invalid drawing type.", drawing_object).c_str());
		return 0;
	}

	return 1;
}
std::int32_t drawing_environment::is_render_property(misako_State* state)
{
	const auto drawing_property = reinterpret_cast<base_t*>(lua_touserdata(state, 1));
	const auto drawing_type = drawing_property->type;

	if (drawing_type == line || drawing_type == circle || drawing_type == square || drawing_type == quad || drawing_type == image)
		lua_pushboolean(state, 1);
	else
		lua_pushboolean(state, 0);

	return 1;
}
std::int32_t drawing_environment::get_render_property(misako_State* state)
{
	const auto drawing_property = reinterpret_cast<base_t*>(lua_touserdata(state, 1));

	switch (drawing_property->type)
	{
	case line:
	{
		drawing_line::index(state);
		break;
	}
	case circle:
	{
		drawing_circle::index(state);
		break;
	}
	case square:
	{
		drawing_square::index(state);
		break;
	}
	case triangle:
	{
		drawing_triangle::index(state);
		break;
	}
	case quad:
	{
		drawing_quad::index(state);
		break;
	}
	case text:
	{
		drawing_text::index(state);
		break;
	}
	case image:
	{
		drawing_image::index(state);
		break;
	}
	default:
		break;
	}

	return 1;
}
std::int32_t drawing_environment::set_render_property(misako_State* state)
{
	const auto drawing_property = reinterpret_cast<base_t*>(lua_touserdata(state, 1));

	switch (drawing_property->type)
	{
	case line:
	{
		drawing_line::newindex(state);
		break;
	}
	case circle:
	{
		drawing_circle::newindex(state);
		break;
	}
	case square:
	{
		drawing_square::newindex(state);
		break;
	}
	case triangle:
	{
		drawing_triangle::newindex(state);
		break;
	}
	case quad:
	{
		drawing_quad::newindex(state);
		break;
	}
	case text:
	{
		drawing_text::newindex(state);
		break;
	}
	case image:
	{
		drawing_image::newindex(state);
		break;
	}
	default:
		break;
	}

	return 1;
}

void drawing_environment::setup_drawing_environment(misako_State* state)
{
	luaL_newmetatable(state, "drawline");
	misako_register_table(state, drawing_line::index, "__index");
	misako_register_table(state, drawing_line::newindex, "__newindex");

	luaL_newmetatable(state, "drawcircle");
	misako_register_table(state, drawing_circle::index, "__index");
	misako_register_table(state, drawing_circle::newindex, "__newindex");

	luaL_newmetatable(state, "drawsquare");
	misako_register_table(state, drawing_square::index, "__index");
	misako_register_table(state, drawing_square::newindex, "__newindex");

	luaL_newmetatable(state, "drawtriangle");
	misako_register_table(state, drawing_triangle::index, "__index");
	misako_register_table(state, drawing_triangle::newindex, "__newindex");

	luaL_newmetatable(state, "drawquad");
	misako_register_table(state, drawing_quad::index, "__index");
	misako_register_table(state, drawing_quad::newindex, "__newindex");

	luaL_newmetatable(state, "drawtext");
	misako_register_table(state, drawing_text::index, "__index");
	misako_register_table(state, drawing_text::newindex, "__newindex");

	// TO-DO: verify Drawing.image works, i just threw in shit that was shown in SW docs -Rexi
	luaL_newmetatable(state, "drawimage");
	misako_register_table(state, drawing_image::index, "__index");
	misako_register_table(state, drawing_image::newindex, "__newindex");

	lua_newtable(state);
	{
		misako_register_table(state, drawing_environment::drawing_new, "new");
		misako_register_table(state, drawing_base::clear, "clear");

		lua_newtable(state);
		{
			lua_pushnumber(state, 0);
			lua_setfield(state, -2, "UI");
			lua_pushnumber(state, 1);
			lua_setfield(state, -2, "System");
			lua_pushnumber(state, 2);
			lua_setfield(state, -2, "Plex");
			lua_pushnumber(state, 3);
			lua_setfield(state, -2, "Monospace");
		}
		lua_setfield(state, -2, "Fonts");
	}
	lua_setfield(state, -10002, "Drawing");

	misako_register(state, drawing_base::clear, "cleardrawcache");

	misako_register(state, drawing_environment::is_render_property, "isrenderobj");
	misako_register(state, drawing_environment::get_render_property, "getrenderproperty");
	misako_register(state, drawing_environment::set_render_property, "setrenderproperty");
}