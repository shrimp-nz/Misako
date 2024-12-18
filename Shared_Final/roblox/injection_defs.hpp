#pragma once
#include <Windows.h>

#define UNLINK(x) (x).Blink->Flink = (x).Flink; \
    (x).Flink->Blink = (x).Blink;

enum _LDR_DLL_LOAD_REASON
{
    LoadReasonStaticDependency,
    LoadReasonStaticForwarderDependency,
    LoadReasonDynamicForwarderDependency,
    LoadReasonDelayloadDependency,
    LoadReasonDynamicLoad,
    LoadReasonAsImageLoad,
    LoadReasonAsDataLoad,
    LoadReasonEnclavePrimary,
    LoadReasonEnclaveDependency,
    LoadReasonPatchImage,
    LoadReasonUnknown
};

typedef struct _LDR_DATA_TABLE_ENTRY_
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    HMODULE DllBase;
    uint32_t EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    union {
        UCHAR FlagGroup[4];
        ULONG Flags;
        struct {
            UCHAR PackagedBinary : 1;
            UCHAR MarkedForRemoval : 1;
            UCHAR ImageDll : 1;
            UCHAR LoadNotificationsSent : 1;
            UCHAR TelemetryEntryProcessed : 1;
            UCHAR ProcessStaticImport : 1;
            UCHAR InLegacyLists : 1;
            UCHAR InIndexes : 1;
            UCHAR ShimDll : 1;
            UCHAR InExceptionTable : 1;
            UCHAR ReservedFlags1 : 2;
            UCHAR LoadInProgress : 1;
            UCHAR LoadConfigProcessed : 1;
            UCHAR EntryProcessed : 1;
            UCHAR ProtectDelayLoad : 1;
            UCHAR ReservedFlags3 : 2;
            UCHAR DontCallForThreads : 1;
            UCHAR ProcessAttachCalled : 1;
            UCHAR ProcessAttachFailed : 1;
            UCHAR CorDeferredValidate : 1;
            UCHAR CorImage : 1;
            UCHAR DontRelocate : 1;
            UCHAR CorILOnly : 1;
            UCHAR ChpeImage : 1;
            UCHAR ChpeEmulatorImage : 1;
            UCHAR ReservedFlags5 : 1;
            UCHAR Redirected : 1;
            UCHAR ReservedFlags6 : 2;
            UCHAR CompatDatabaseProcessed : 1;
        };
    };
    USHORT ObsoleteLoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    ULONG TimeDateStamp;
    uint32_t EntryPointActivationContext;
    uint32_t Lock;
    uint32_t DdagNode;
    LIST_ENTRY NodeModuleLink;
    uint32_t LoadContext;
    uint32_t ParentDllBase;
    uint32_t SwitchBackContext;
    uint32_t BaseAddressIndexNode;
    uint32_t MappingInfoIndexNode;
    ULONG OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG BaseNameHashValue;
    _LDR_DLL_LOAD_REASON LoadReason;
    ULONG ImplicitPathOptions;
    ULONG ReferenceCount;
    ULONG DependentLoadFlags;
    UCHAR SigningLevel;
    ULONG CheckSum;
    uint32_t ActivePatchImageBase;
    uint32_t HotPatchState;
} LDR_DATA_TABLE_ENTRY_, * PLDR_DATA_TABLE_ENTRY_;

typedef struct _ProcessModuleInfo
{
    unsigned int size;
    unsigned int initialized;
    HANDLE SsHandle;
    LIST_ENTRY LoadOrder;
    LIST_ENTRY InitOrder;
    LIST_ENTRY MemoryOrder;
} ProcessModuleInfo, * pProcessModuleInfo;

#pragma comment(linker, "/export:MiniDumpWriteDump=_MiniDumpWriteDump@28")
extern __declspec(dllexport) BOOL WINAPI MiniDumpWriteDump(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, void* DumpType, void* ExceptionParam, void* UserStreamParam, void* CallbackParam) {
    // blah
    return false;
}

void RemoveLinkedMod(HMODULE hMod)
{
    ProcessModuleInfo* pmInfo;
    LDR_DATA_TABLE_ENTRY_* module;

    _asm
    {
        mov eax, fs: [18h]       // TEB
        mov eax, [eax + 30h]    // PEB
        mov eax, [eax + 0Ch]    // PROCESS_MODULE_INFO
        mov pmInfo, eax
    }

    module = (LDR_DATA_TABLE_ENTRY_*)(pmInfo->LoadOrder.Flink);
    while (module->DllBase && module->DllBase != hMod) {
        module = (LDR_DATA_TABLE_ENTRY_*)(module->InLoadOrderLinks.Flink);
    }

    if (!module->DllBase)
        return;

    module->SizeOfImage = 0;
    module->DllBase = 0;
    module->TimeDateStamp = 0;


    UNLINK(module->InLoadOrderLinks);
    UNLINK(module->InInitializationOrderLinks);
    UNLINK(module->InMemoryOrderLinks);
    UNLINK(module->HashLinks);
    UNLINK(module->NodeModuleLink);

    memset(module, 0, sizeof(module));
}