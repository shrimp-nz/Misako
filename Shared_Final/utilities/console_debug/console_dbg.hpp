#pragma once
#include <string>
#include <vector>

#include <format>
#include <iostream>

#include <roblox/config.hpp>
#include <dependencies/xorstring/xorstring.hpp>

#include <Windows.h>

enum log_type : int
{
	print = 255,
	warn = 102,
	error = 68,
	info = 51,
	debug = 85
};

class console_t
{

	HANDLE hConsole;

public:
	bool hidden = true;
	console_t(const char* const name);
	void hide();
	void show();

	template<log_type t, typename... args_t>
	void write(const std::vector<const char*>& caller, const char* fmt, args_t... args)
	{
		SetConsoleTextAttribute(hConsole, t);
		std::cout << "  ";
		SetConsoleTextAttribute(hConsole, 7);

		std::cout << xorstr(" [");

		for (auto& c : caller)
		{
			std::cout << c;
			if (c != caller.back())
				std::cout << xorstr("->");
		}

		std::cout << xorstr("]: ");

		std::printf(fmt, args...);

		std::cout << std::endl;
	}
};

extern console_t console;