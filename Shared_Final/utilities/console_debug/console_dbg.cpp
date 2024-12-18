#include <utilities/console_debug/console_dbg.hpp>


#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

console_t console{xorstr("Mistinky debug console")};

console_t::console_t(const char* const name)
{
    const auto lib = LoadLibraryW(xorstr(L"KERNEL32.dll"));
    const auto free_console = reinterpret_cast<std::uintptr_t>(GetProcAddress(lib, xorstr("FreeConsole")));

    if (free_console)
    {
        static auto jmp = free_console + 0x6;
        DWORD old{};

        VirtualProtect(reinterpret_cast<void*>(free_console), 0x6, PAGE_EXECUTE_READWRITE, &old);
        *reinterpret_cast<std::uintptr_t**>(free_console + 0x2) = &jmp;
        *reinterpret_cast<std::uint8_t*>(free_console + 0x6) = 0xC3;
        VirtualProtect(reinterpret_cast<void*>(free_console), 0x6, old, &old);
    }

    AllocConsole();

    FILE* file_stream;

    freopen_s(&file_stream, xorstr("CONIN$"), "r", stdin);
    freopen_s(&file_stream, xorstr("CONOUT$"), "w", stdout);
    freopen_s(&file_stream, xorstr("CONOUT$"), "w", stderr);

    SetConsoleTitleA(name);

    hConsole = GetStdHandle(-11);
}

void console_t::hide()
{
    console.hidden = true;
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void console_t::show()
{
    console.hidden = false;
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}