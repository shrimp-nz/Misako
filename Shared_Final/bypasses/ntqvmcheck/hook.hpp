#pragma once
#include <dependencies/minhook/MinHook.h>
#include <iostream>

class _NtQVMHk {
    HMODULE ntdll;
    FARPROC NTQVM_adr;

    typedef enum _MEMORY_INFORMATION_CLASS {
        MemoryBasicInformation
    } MEMORY_INFORMATION_CLASS;

    inline static NTSTATUS(__stdcall* orig)(HANDLE, PVOID, MEMORY_INFORMATION_CLASS, PVOID, SIZE_T, PSIZE_T);

    static NTSTATUS HookStub(HANDLE ProcessHandle, PVOID BaseAddress,
        MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation,
        SIZE_T MemoryInformationLength, PSIZE_T ReturnLength)
    {
        //auto ret = orig(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
        return STATUS_ACCESS_VIOLATION;
    }

public:

    _NtQVMHk() : ntdll(LoadLibrary("ntdll.dll")), NTQVM_adr(GetProcAddress(ntdll, "NtQueryVirtualMemory")) {
        MH_Initialize();
        MH_CreateHook(NTQVM_adr, &HookStub, reinterpret_cast<LPVOID*>(&orig));
        MH_EnableHook(NTQVM_adr);
    }
};