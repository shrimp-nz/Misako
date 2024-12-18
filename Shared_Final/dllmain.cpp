#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <iostream>


#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <string>
#include "roblox/injection_defs.hpp"

#include <cstdint>
#include <cstddef>

#include <thread>
#include <queue>

/* configs */
#include <roblox/config.hpp>
#include <roblox/offsets.hpp>

/* third party */
#include <dependencies/xorstring/xorstring.hpp>
#include <dependencies/task_scheduler/task_scheduler.hpp>

/* utilities */
#include <utilities/console_debug/console_dbg.hpp>
#include <utilities/tools/tools.hpp>

/* bypasses */
#include <bypasses/callcheck/callcheck.hpp>
#include <bypasses/memcheck/memcheck.hpp>
#include <bypasses/trustcheck/trustcheck.hpp>
#include <bypasses/ntqvmcheck/hook.hpp>

/* environment */
#include <environment/custom/custom.hpp>
#include <environment/debug/debug.hpp>
#include <environment/drawing/drawing.hpp>
#include <environment/websocket/websocket.hpp>
#include <environment/connection/connection.hpp>
#include <environment/file/file.hpp>
#include <environment/crypt/crypt.hpp>
#include <environment/input/input.hpp>
#include <environment/console/console.hpp>

/* luau */
#include <luau/Compiler.h>
#include <Luau/BytecodeBuilder.h>

/* renderer */
#include <dependencies/renderer/dx11_renderer.hpp>

task_scheduler scheduler;
memcheck_t meme;

std::queue<std::string> scheduled_script;
std::queue<std::string> after_teleport_script;

namespace states
{
    std::uintptr_t datamodel;
    std::uintptr_t script_context;
    std::uintptr_t scheduler_state;
    misako_State* global_state;
    misako_State* lua_state;
}

struct live_thread_ref
{
    int unk_0;
    misako_State* state;
    int state_id; // thread_id
    int unk_1;
};

struct thread_ref
{ 
    live_thread_ref* ref;
    thread_ref(misako_State* L)
    {
        ref = new live_thread_ref;
        ref->unk_0 = 0;
        ref->state = L;
        lua_pushthread(L);
        ref->state_id = lua_ref(L, -1);
        lua_settop(L, 0);

        ref->unk_1 = 0;
    }
};

namespace main
{
    void misako_states()
    {
        states::datamodel = scheduler.get_data_model();
        states::script_context = scheduler.get_script_context();

        states::scheduler_state = r_set_lua_state(states::script_context);
        states::global_state = reinterpret_cast<misako_State*>(states::scheduler_state);
        states::lua_state = lua_newthread(states::global_state);

        luaL_sandboxthread(states::lua_state);

        set_thread_context(reinterpret_cast<const std::uintptr_t>(states::lua_state), 8);
    }

    bool execute(const std::string& script)
    {
        const auto& compile = MisakoRVM::compile(script);
        const auto state = lua_newthread(states::lua_state);

        luaL_sandboxthread(state);

        thread_ref thread{state};

        if (luau_load(thread.ref->state, tools::io_addons::random_string(5).data(), compile.data(), compile.length(), 0))
        {
            lua_getfield(thread.ref->state, -10002, "warn");
            lua_insert(thread.ref->state, -2);
            lua_pcall(thread.ref->state, 1, 0, 0);
            return false;
        }

        reinterpret_cast<int(__thiscall*)(std::uintptr_t, thread_ref*, int, bool, std::string*)>(scriptcontext_resume_address)(states::script_context, &thread, 0, false, nullptr);
        return true;
    }

    void misako_bypasses()
    {
        meme.initialize_bypass();
        callcheck::bypass();
        trustcheck::bypass();
    }

    __forceinline void http_services()
    {
        lua_getglobal(states::lua_state, "game");
        lua_getfield(states::lua_state, -1, "HttpGetAsync");
        lua_setglobal(states::lua_state, "HttpGetAsync");
    }

    __forceinline void misako_environment()
    {
        environment::setup_custom_environment(states::lua_state);
        io_environment::setup_io_environment(states::lua_state);
        console_environment::setup_console_environment(states::lua_state);

        lua_newtable(states::lua_state);
        lua_setglobal(states::lua_state, "shared");
        lua_newtable(states::lua_state);
        lua_setglobal(states::lua_state, "_G");

        debug_environment::setup_debug_environment(states::lua_state);
        input_environment::setup_input_environment(states::lua_state);
        crypt_environment::setup_crypt_environment(states::lua_state);
        websocket_environment::setup_websocket_environment(states::lua_state);
        drawing_environment::setup_drawing_environment(states::lua_state);
    }

    void teleport_handler()
    {
        auto datamodel = scheduler.get_data_model();

        while (true)
        {
            Sleep(500);

            if (datamodel != scheduler.get_data_model())
            {
                while (!scheduler.is_game_loaded())
                    Sleep(10);

                main::misako_states();
                datamodel = states::datamodel;

                scheduler.hook_scripts_job();

                main::misako_environment();
                main::http_services();
                scheduled_script.push(tools::internet_addons::read_website("http://misako.protogen.lol/scr/simplehook.lua"));

                if (!after_teleport_script.empty())
                {
                    for (unsigned i = 0; i < after_teleport_script.size(); i++)
                    {
                        scheduled_script.push(after_teleport_script.front());
                        after_teleport_script.pop();
                    }
                }
            }
        }
    }

    __forceinline void pipe_communication()
    {
        std::string script_buffer;
        char readbuffer[999999];
        DWORD read;
        HANDLE handle = CreateNamedPipeW(TEXT(xorstr(L"\\\\.\\pipe\\MisakoConnection")), PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, PIPE_WAIT, 1, 999999, 999999, NMPWAIT_USE_DEFAULT_WAIT, 0);
        while (handle != INVALID_HANDLE_VALUE)
        {
            if (ConnectNamedPipe(handle, 0))
            {
                while (ReadFile(handle, readbuffer, sizeof(readbuffer) - 1, &read, 0))
                {
                    readbuffer[read] = '\0';
                    script_buffer.append(readbuffer);
                }

                scheduled_script.push(script_buffer);
                script_buffer.clear();
            }

            DisconnectNamedPipe(handle);
        }
    }
}

std::int32_t main_thread()
{
    try
    {
        scheduler.hook_scripts_job();
        console.write< log_type::debug >({xorstr("initialization"), xorstr("debug")}, xorstr("hooked scripts job"));

        main::misako_states();
        console.write< log_type::debug >({ xorstr("initialization"), xorstr("debug") }, xorstr("states"));

        main::misako_bypasses();
        console.write< log_type::debug >({ xorstr("initialization"), xorstr("debug") }, xorstr("bypasses"));

        main::misako_environment();
        console.write< log_type::debug >({ xorstr("initialization"), xorstr("debug") }, xorstr("environment"));

        main::http_services();
        console.write< log_type::debug >({ xorstr("initialization"), xorstr("debug") }, xorstr("http services"));

        //scheduled_script.push(tools::internet_addons::read_website("http://misako.protogen.lol/scr/simplehook.lua"));
        //console.write< log_type::debug >({ xorstr("initialization"), xorstr("debug") }, xorstr("script"));
    }
    catch (...) 
    {

    }

    std::thread{ main::teleport_handler }.detach();
    std::thread{ drawing_renderer::hook }.detach();
    std::thread{ main::pipe_communication }.detach();

    return 1;
}

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    //_NtQVMHk hook;
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        if constexpr (auto_launch)
        {
            PTEB teb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
            PLIST_ENTRY head = &teb->ProcessEnvironmentBlock->Ldr->InMemoryOrderModuleList, next;
            for (next = head->Flink; next != head; next = next->Flink)
            {
                if (wcsstr(reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(next)->FullDllName.Buffer, L"dbghelp") != 0)
                {
                    next->Flink->Blink = next->Blink;
                    next->Blink->Flink = next->Flink;
                    console.show();
                    std::thread(main_thread).detach();
                }
            }

            DWORD old_protect;

            MEMORY_BASIC_INFORMATION mbi;
            VirtualQuery(hinstDLL, &mbi, sizeof(mbi));
            VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &old_protect);
            ZeroMemory(hinstDLL, 4096);
            VirtualProtect(mbi.BaseAddress, mbi.RegionSize, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), mbi.BaseAddress, mbi.RegionSize);

            auto pDoH = reinterpret_cast<PIMAGE_DOS_HEADER>(hinstDLL);
            auto pNtH = reinterpret_cast<PIMAGE_NT_HEADERS>(hinstDLL + pDoH->e_lfanew);
            if (VirtualProtect(pDoH, sizeof(IMAGE_DOS_HEADER), PAGE_READWRITE, &old_protect))
                ZeroMemory(pDoH, sizeof(IMAGE_DOS_HEADER));
            if (pNtH && VirtualProtect(pNtH, sizeof(IMAGE_NT_HEADERS), PAGE_READWRITE, &old_protect))
                ZeroMemory(pNtH, sizeof(IMAGE_NT_HEADERS));

            RemoveLinkedMod(hinstDLL);
        }
        else
            std::thread{main_thread}.detach();
    }

    return TRUE;
}
 