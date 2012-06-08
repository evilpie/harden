#include <windows.h>

typedef NTSTATUS (NTAPI *_NtSetInformationProcess)(
    HANDLE ProcessHandle,
    PROCESS_INFORMATION_CLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength);

/*
 * Enable or disable DEP for the current process.
 * Once you use used this with Permanent the option,
 *  the DEP setting can't be changed.
 * You should not call this function on x64 unless you are in an Wow64Process, 
 *  because DEP is always enabled.
 */
bool
SetDEP(bool Enable, bool Permanent) 
{
    _NtSetInformationProcess NtSetInformationProcess = 
        (_NtSetInformationProcess *)(GetProcAddress(GetModuleHandle("ntdll"), "NtSetInformationProcess"));

    if (!NtSetInformationProcess)
        return false;

    ULONG Flags = Enable ? MEM_EXECUTE_OPTION_ENABLE : MEM_EXECUTE_OPTION_DISABLE;

    if (Permanent) {
        Flags |= MEM_EXECUTE_OPTION_PERMANENT;
    }

    NTSTATUS Status = NtSetInformationProcess(GetCurrentProcess(), ProcessExecuteFlags, &flags, sizeof(ULONG));
    return NT_SUCCESS(Status);
}

/*
 * ASLR works by allocating the preferred image base of an to be loaded image,
 * this forces the windows kernel to load it at a different address.
 */
bool
SetMandoryASLR(bool Enable)
{
    return false;
}

/*
 * This increases the entropy of DLL base addresses even further.
 * EMET seems to disable this for Vista and up.
 */
void
EnableBottomUpRand()
{
    ULONG Seed = GetCurrentProcessId() ^ GetTickCount();
    ULONG Range = RtlRandom(&Seed) & 255;

    for (ULONG i = 0; i < Range, i++) {
        PVOID Address = NULL;
        SIZE_T Size = 64 * 1024;
        (void) NtAllocateVirtualMemory(GetCurrentProcess(), Address, 0, &Size, MEM_RESERVE, PAGE_NOACCESS);
    }
}

bool
SetUpProtection()
{
    return true;
}
